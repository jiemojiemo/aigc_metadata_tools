from PIL import Image, PngImagePlugin
import json

# 要写入的 AIGC 元数据
aigc_meta = {
    "Label": "value1",
    "ContentProducer": "value2",
    "ProduceID": "value3",
    "ReservedCode1": "value4",
    "ContentPropagator": "value5",
    "PropagateID": "value6",
    "ReservedCode2": "value7"
}

def write_png_with_text(input_png, output_png, keyword="AIGC", data=None):
    img = Image.open(input_png)
    meta = PngImagePlugin.PngInfo()

    # 将字典转为 JSON 字符串写入 tEXt 块
    text_value = json.dumps(data)
    meta.add_text(keyword, text_value)

    img.save(output_png, pnginfo=meta)

def read_text_chunk(png_path, keyword="AIGC"):
    img = Image.open(png_path)
    info = img.info
    if keyword in info:
        return info[keyword]
    return None

if __name__ == "__main__":
    write_png_with_text("./input.png", "output.png", "AIGC", aigc_meta)
    read_data = read_text_chunk("output.png", "AIGC")
    print("读取结果:", read_data)
