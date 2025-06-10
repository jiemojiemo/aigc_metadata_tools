import argparse
import json

import piexif
from PIL import Image
import pillow_heif

aigc_meta = {
    "Label": "value1",
    "ContentProducer": "value2",
    "ProduceID": "value3",
    "ReservedCode1": "value4",
    "ContentPropagator": "value5",
    "PropagateID": "value6",
    "ReservedCode2": "value7"
}

def read_user_comment_from_heif(heif_path):
    img = Image.open(heif_path)
    exif_data = img.info.get('exif', {})
    exif_dict = piexif.load(exif_data)
    user_comment_bytes = exif_dict["Exif"].get(piexif.ExifIFD.UserComment)
    if not user_comment_bytes:
        print("没有 UserComment 字段")
        exit()
    encoding = user_comment_bytes[:8].decode('ASCII').rstrip('\0')  # 解析编码方式
    if encoding.startswith("ASCII"):
        user_comment = user_comment_bytes[8:].split(b'\x00',1)[0].decode("ascii")
    elif encoding.startswith("UNICODE"):
        user_comment = user_comment_bytes[8:].decode("utf-16-be")
    else:
        raise ValueError("Unsupported encoding in UserComment")
    return user_comment

def make_user_comment_bytes(comment, encoding="ascii"):
    if encoding.lower() == "ascii":
        prefix = b'ASCII\x00\x00\x00'
        return prefix + comment.encode('ascii')
    elif encoding.lower() == "unicode":
        prefix = b'UNICODE\x00'
        # UCS2 (每两个字节一个字符)
        return prefix + comment.encode('utf-16-be')
    else:
        raise ValueError("Unsupported encoding")

def write_user_comment_to_heic(path_to_heif_file, path_to_output, user_comment):
    # 注册 HEIF/HEIC 支持（新版 Pillow 可省略，但加上更保险）
    pillow_heif.register_heif_opener()

    # 打开原始图片
    img = Image.open(path_to_heif_file)

    # 构造 EXIF: UserComment 属于 Exif IFD (tag 0x9286)
    comment_bytes = make_user_comment_bytes(user_comment)
    exif_dict = {"Exif": {piexif.ExifIFD.UserComment: comment_bytes}}
    exif_bytes = piexif.dump(exif_dict)

    # 保存图片并写入EXIF
    img.save(path_to_output, exif=exif_bytes)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="HEIF read/write example")
    parser.add_argument("--input", type=str, required=True, help="Input HEIF file")
    parser.add_argument("--output", type=str, required=True, help="Output HEIF file")
    args = parser.parse_args()

    pillow_heif.register_heif_opener()

    aigc_meta_json = json.dumps(aigc_meta)
    write_user_comment_to_heic(args.input, args.output, aigc_meta_json)
    exif_data = read_user_comment_from_heif(args.output)
    print("读取的数据:", exif_data)