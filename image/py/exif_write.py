import json
import piexif
from PIL import Image
import pillow_heif

import os

from exif_read import read_exif_user_comment

script_dir = os.path.dirname(os.path.abspath(__file__))

aigc_meta = {
    "Label": "value1",
    "ContentProducer": "value2",
    "ProduceID": "value3",
    "ReservedCode1": "value4",
    "ContentPropagator": "value5",
    "PropagateID": "value6",
    "ReservedCode2": "value7"
}
aigc_meta_dict = {
    'AIGC':aigc_meta
}

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

def write_to_exif_user_comment(input_path, path_to_output, user_comment):
    # 注册 HEIF/HEIC 支持（新版 Pillow 可省略，但加上更保险）
    pillow_heif.register_heif_opener()

    # 打开原始图片
    img = Image.open(input_path)

    # 构造 EXIF: UserComment 属于 Exif IFD (tag 0x9286)
    comment_bytes = make_user_comment_bytes(user_comment)
    exif_dict = {"Exif": {piexif.ExifIFD.UserComment: comment_bytes}}
    exif_bytes = piexif.dump(exif_dict)

    # 保存图片并写入EXIF
    img.save(path_to_output, exif=exif_bytes)

if __name__ == "__main__":
    input_images = [
        os.path.join(script_dir, "../../test_resources/heic_exif_empty.heic"),
        os.path.join(script_dir, "../../test_resources/heif_exif_empty.heif"),
        os.path.join(script_dir, "../../test_resources/jpg_exif_empty.jpg"),
        os.path.join(script_dir, "../../test_resources/png_exif_empty.png"),
        os.path.join(script_dir, "../../test_resources/webp_exif_empty.webp"),
    ]

    for image_path in input_images:
        ext = os.path.splitext(image_path)[1].lower()
        output_path = os.path.join(script_dir, f"output{ext}")
        print(f"正在将 AIGC 信息写入: {output_path}")
        write_to_exif_user_comment(image_path, output_path, json.dumps(aigc_meta_dict))
        print(f"已将 AIGC 信息写入: {output_path}")

        # 读取并验证写入的数据
        user_comment = read_exif_user_comment(output_path)
        user_comment_dict = json.loads(user_comment)
        assert user_comment_dict == aigc_meta_dict, f"读取的元数据与预期不符: {user_comment_dict} != {aigc_meta_dict}"
