import json

import piexif
from PIL import Image
import pillow_heif
import os
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

def read_exif_user_comment(image_path):
    img = Image.open(image_path)
    exif_data = img.info.get('exif', {})
    exif_dict = piexif.load(exif_data)
    user_comment_bytes = exif_dict["Exif"].get(piexif.ExifIFD.UserComment)
    if not user_comment_bytes:
        print("没有 UserComment 字段")
        return ""
    encoding = user_comment_bytes[:8].decode('ASCII').rstrip('\0')  # 解析编码方式
    if encoding.startswith("ASCII"):
        user_comment = user_comment_bytes[8:].split(b'\x00',1)[0].decode("ascii")
    elif encoding.startswith("UNICODE"):
        user_comment = user_comment_bytes[8:].decode("utf-16-be")
    else:
        user_comment = user_comment_bytes[8:].split(b'\x00',1)[0].decode("ascii")
    return user_comment

if __name__ == "__main__":
    input_images = [
        os.path.join(script_dir, "../../test_resources/heic_exif_aigc.heic"),
        os.path.join(script_dir, "../../test_resources/heif_exif_aigc.heif"),
        os.path.join(script_dir, "../../test_resources/jpg_exif_aigc.jpg"),
        os.path.join(script_dir, "../../test_resources/png_exif_aigc.png"),
        os.path.join(script_dir, "../../test_resources/webp_exif_aigc.webp"),
    ]

    pillow_heif.register_heif_opener()

    for image_path in input_images:
        print(f"正在读取图片: {image_path}")
        user_comment = read_exif_user_comment(image_path)
        print("读取的数据:", user_comment)
        user_comment_dict = json.loads(user_comment)
        assert user_comment_dict == aigc_meta_dict, f"读取的元数据与预期不符: {user_comment_dict} != {aigc_meta_dict}"