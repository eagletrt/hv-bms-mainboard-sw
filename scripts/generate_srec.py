Import("env")
import os
import re
import glob

# The strict flags you only want for Core/ code
STRICT_WARNINGS = [
    "-Wall",
    "-Wextra",
    "-Wpedantic"
]

def apply_core_warnings(env, node):
    # Replace backslashes to ensure compatibility (Windows = bad)
    path = node.get_path().replace("\\", "/")

    # Check if the source file is inside the Core/ directory
    if "Core/" in path:
        return env.Object(
            node,
            CCFLAGS=env.get("CCFLAGS", []) + STRICT_WARNINGS
        )

    return node


env.AddBuildMiddleware(apply_core_warnings)

board = env.BoardConfig()

ldscript = env.GetProjectOption("board_build.ldscript", board.get("build.ldscript", ""))
if ldscript and not os.path.isabs(ldscript):
    ldscript = os.path.join(env.subst("$PROJECT_DIR"), ldscript)

def get_flash_origin(ldscript_path):
    if not ldscript_path or not os.path.isfile(ldscript_path):
        raise Exception("ldscript not found: %s" % ldscript_path)
    with open(ldscript_path, "r") as f:
        content = f.read()
    match = re.search(
        r'FLASH\s*\([^)]*\)\s*:\s*ORIGIN\s*=\s*(0x[0-9A-Fa-f]+)', content
    )
    if not match:
        raise Exception("Could not find FLASH ORIGIN in %s" % ldscript_path)
    return match.group(1)


def bin_to_srec(source, target, env):
    flash_addr = get_flash_origin(ldscript)
    print("Detected FLASH origin address: %s" % flash_addr)

    bin_path = str(target[0])
    srec_path = os.path.join(
        os.path.dirname(bin_path), env.subst("${PROGNAME}.srec")
    )

    cmd = [env.subst("$OBJCOPY"), "-I", "binary", "-O", "srec",
           "--adjust-vma=" + flash_addr, bin_path, srec_path]
    print("Running:", " ".join(cmd))
    env.Execute(env.VerboseAction(" ".join(cmd), "Converting BIN to SREC"))


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", bin_to_srec)


def detect_can_device():
    """Fall back to the first SocketCAN interface found under /sys/class/net."""
    candidates = sorted(glob.glob("/sys/class/net/*/type"))
    for type_file in candidates:
        try:
            with open(type_file) as f:
                # ARPHRD_CAN == 280
                if f.read().strip() == "280":
                    return type_file.split("/")[-2]
        except OSError:
            continue
    return None


def build_bootcommander_cmd(env):
    bootcommander = env.GetProjectOption("custom_bootcommander_path", "./bootcommander")
    device = env.GetProjectOption("custom_can_device", "") or detect_can_device()
    if not device:
        raise Exception(
            "No CAN device specified (custom_can_device) and none could be "
            "auto-detected on this system."
        )

    id_flash = env.GetProjectOption("custom_can_id_flash", "")
    id_tx = env.GetProjectOption("custom_can_id_tx", "")
    id_rx = env.GetProjectOption("custom_can_id_rx", "")
    baud = env.GetProjectOption("custom_can_baud", "1000000")

    if not id_tx or not id_rx:
        raise Exception(
            "custom_can_id_tx / custom_can_id_rx must be set (in platformio.ini "
            "or via --project-option) before uploading."
        )

    srec_path = "$BUILD_DIR/${PROGNAME}.srec"

    cmd = (
        'cansend {dev} {fid}#01 && '
        '{bc} -t=xcp_can -t6=200 -d={dev} -b={baud} '
        '-tid={tid} -rid={rid} {srec}'
    ).format(
        bc=bootcommander,
        dev=device,
        baud=baud,
        fid=id_flash.zfill(3),
        tid=id_tx,
        rid=id_rx,
        srec=env.subst(srec_path),
    )
    return cmd


env.Replace(
    UPLOADER="bootcommander",
    UPLOADCMD=build_bootcommander_cmd(env),
)
