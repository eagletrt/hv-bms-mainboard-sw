Import("env")

import struct
import subprocess

# Must match the bootloader: first user sector in flashLayout[] and
# BOOT_FLASH_VECTOR_TABLE_CS_OFFSET in
# hv-bms-mainboard-bootloader-sw/Core/Src/openblt/flash.c.
APP_BASE_ADDRESS = 0x08008000
CHECKSUM_OFFSET = 0x188

# The checksum lands on vector table entry 98 (0x188 / 4), a reserved slot
# on the STM32F446, so no extra space is needed in the linker script.
VECTOR_TABLE_SYMBOL = "g_pfnVectors"


def symbol_address(elf_path, name):
    nm = env.subst("$OBJCOPY").replace("objcopy", "nm")
    output = subprocess.run([nm, elf_path], check=True, capture_output=True, text=True).stdout
    for line in output.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[2] == name:
            return int(parts[0], 16)
    return None


def write_openblt_checksum(source, target, env):
    """
    Writes the OpenBLT signature checksum into firmware.bin and generates firmware.srec.

    The bootloader only starts the main firmware if the word at CHECKSUM_OFFSET plus the
    first 7 vector table entries adds up to 0. It writes this value itself when flashing
    over CAN, but not when the image is flashed through SWD, so it is precomputed here.
    The .srec is the file format BootCommander/MicroBoot expect.
    """
    bin_path = target[0].get_abspath()
    elf_path = env.subst("$BUILD_DIR/${PROGNAME}.elf")

    address = symbol_address(elf_path, VECTOR_TABLE_SYMBOL)
    if address != APP_BASE_ADDRESS:
        raise RuntimeError(
            "the vector table (%s) is at %s, the bootloader expects it at 0x%08X: "
            "check board_build.ldscript"
            % (VECTOR_TABLE_SYMBOL, hex(address) if address is not None else "nowhere", APP_BASE_ADDRESS)
        )

    with open(bin_path, "rb") as f:
        image = bytearray(f.read())

    checksum = (-sum(struct.unpack_from("<7I", image, 0))) & 0xFFFFFFFF
    current = struct.unpack_from("<I", image, CHECKSUM_OFFSET)[0]
    if current not in (0, checksum):
        raise RuntimeError(
            "the word at offset 0x%X is 0x%08X, not a reserved vector: "
            "the checksum would overwrite an interrupt handler" % (CHECKSUM_OFFSET, current)
        )

    struct.pack_into("<I", image, CHECKSUM_OFFSET, checksum)
    with open(bin_path, "wb") as f:
        f.write(image)
    print("OpenBLT checksum 0x%08X written to %s" % (checksum, bin_path))

    srec_path = bin_path[: -len(".bin")] + ".srec"
    env.Execute(
        env.VerboseAction(
            '"$OBJCOPY" -I binary -O srec --srec-forceS3 --change-addresses 0x%08X "%s" "%s"'
            % (APP_BASE_ADDRESS, bin_path, srec_path),
            "Generating %s" % srec_path,
        )
    )


env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", write_openblt_checksum)
