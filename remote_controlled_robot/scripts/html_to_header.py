from pathlib import Path

input_file = Path("web/web_UI.html")
output_file = Path("src/web_UI.h")

data = input_file.read_bytes()

with output_file.open("w", newline="\n") as f:
    f.write("#ifndef web_UI_h\n")
    f.write("#define web_UI_h\n\n")

    f.write("static const unsigned char web_UI[] = {\n")

    for i, b in enumerate(data):
        if i % 12 == 0:
            f.write("    ")

        f.write(f"0x{b:02X}")

        if i != len(data) - 1:
            f.write(", ")

        if (i + 1) % 12 == 0:
            f.write("\n")

    f.write("\n};\n\n")
    f.write(f"static const unsigned int web_UI_LEN = {len(data)};\n\n")
    f.write("#endif\n")

print("Generated include/web_UI.h")
