package com.example.aigcmetadatademo

import android.media.ExifInterface
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.example.aigcmetadatademo.ui.theme.AIGCMetadataDemoTheme
import java.io.File
import java.io.FileOutputStream

class MainActivity : ComponentActivity() {
    private val readAIGCResults = mutableStateListOf<Pair<String, String?>>()
    private val writeAIGCResults = mutableStateListOf<Pair<String, String?>>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            AIGCMetadataDemoTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Column(
                        modifier = Modifier
                            .padding(innerPadding)
                            .padding(16.dp)
                            .verticalScroll(rememberScrollState())
                    ) {
                        ImageFormatSupportText()
                        Spacer(modifier = Modifier.height(24.dp))
                        Text(
                            text = "图片 UserComment 元数据读取结果：",
                            style = MaterialTheme.typography.titleMedium
                        )
                        readAIGCResults.forEach { (imgName, comment) ->
                            Text(
                                text = "${imgName}：${comment ?: "无/读取失败"}",
                                modifier = Modifier.padding(top = 8.dp)
                            )
                        }
                        Text(
                            text = "图片 UserComment 元数据写入结果：",
                            style = MaterialTheme.typography.titleMedium
                        )
                        writeAIGCResults.forEach { (imgName, comment) ->
                            Text(
                                text = "${imgName}：${comment ?: "无/写入失败"}",
                                modifier = Modifier.padding(top = 8.dp)
                            )
                        }
                    }
                }
            }
        }

        readAllAGICMetadata()
        writeAllAGICMetadata()
    }

    private fun readAllAGICMetadata() {
        val inputImages = mapOf(
            "jpg_exif_aigc" to R.raw.jpg_exif_aigc,
            "png_exif_aigc" to R.raw.png_exif_aigc,
            "webp_exif_aigc" to R.raw.webp_exif_aigc,
            "heic_exif_aigc" to R.raw.heic_exif_aigc,
            "heif_exif_aigc" to R.raw.heif_exif_aigc
        )
        readAIGCResults.clear() // 确保每次先清空
        for ((imgName, resId) in inputImages) {
            try {
                resources.openRawResource(resId).use { inputStream ->
                    val exifInterface = ExifInterface(inputStream)
                    val userComment = exifInterface.getAttribute(ExifInterface.TAG_USER_COMMENT)
                    readAIGCResults.add(imgName to userComment)
                }
            } catch (e: Exception) {
                readAIGCResults.add(imgName to "读取失败：${e.message}")
            }
        }
    }

    private fun writeAllAGICMetadata() {
        val ASCII_PREFIX = "ASCII\u0000\u0000\u0000"
        val aigcJsonString = """
             {
               "AIGC": {
                 "Label": "value1",
                 "ContentProducer": "value2",
                 "ProduceID": "value3",
                 "ReservedCode1": "value4",
                 "ContentPropagator": "value5",
                 "PropagateID": "value6",
                 "ReservedCode2": "value7"
               }
             }
        """.trimIndent()
        val outputImages = mapOf(
            "jpg_exif_empty" to R.raw.jpg_exif_empty,
            "png_exif_empty" to R.raw.png_exif_empty,
            "webp_exif_empty" to R.raw.webp_exif_empty,
            "heic_exif_empty" to R.raw.heic_exif_empty,
            "heif_exif_empty" to R.raw.heif_exif_empty
        )
        val userCommentValue = "$ASCII_PREFIX$aigcJsonString"
        for ((imgName, resId) in outputImages) {
            try {
                val cacheFile = File(externalCacheDir, "$imgName")
                resources.openRawResource(resId).use { ins ->
                    FileOutputStream(cacheFile).use { outs ->
                        ins.copyTo(outs)
                    }
                }
                val exifInterface = ExifInterface(cacheFile.absolutePath)
                exifInterface.setAttribute(ExifInterface.TAG_USER_COMMENT, userCommentValue)
                exifInterface.saveAttributes()
                writeAIGCResults.add(imgName to "写入成功")
            } catch (e: Exception) {
                writeAIGCResults.add(imgName to "写入失败：${e.message}")
            }
        }
    }
}



@Composable
fun ImageFormatSupportText(modifier: Modifier = Modifier) {
    Text(
        """
        图片格式支持情况：

        jpg   读：支持 ✅  写：支持 ✅
        png   读：支持 ✅  写：支持 ✅
        webp  读：支持 ✅  写：支持 ✅
        heif  读：支持 ✅  写：不支持 ❌
        heic  读：支持 ✅  写：不支持 ❌
        """.trimIndent(),
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun ImageFormatSupportTextPreview() {
    ImageFormatSupportText()
}