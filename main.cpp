#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <qrencode.h>
#include <png.h>

// 保存QR码为PNG文件
bool saveQRCodeToPNG(const std::string& filename, QRcode* qr, int scale = 10, int margin = 4) {
    if (!qr) return false;

    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return false;
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png_ptr) {
        fclose(fp);
        return false;
    }

    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        fclose(fp);
        return false;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return false;
    }

    png_init_io(png_ptr, fp);

    // 计算图像尺寸
    const int width = qr->width;
    const int image_width = (width + margin * 2) * scale;
    const int image_height = image_width;

    // 设置PNG文件信息
    png_set_IHDR(png_ptr, info_ptr, image_width, image_height,
                 8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    // 创建图像缓冲区
    std::vector<png_byte> row(image_width);

    // 填充上边界（白色）
    for (int y = 0; y < margin * scale; y++) {
        std::fill(row.begin(), row.end(), 255); // 白色
        png_write_row(png_ptr, row.data());
    }

    // 绘制QR码主体
    for (int y = 0; y < width; y++) {
        for (int sy = 0; sy < scale; sy++) { // 垂直缩放
            int x_pos = 0;

            // 左边界（白色）
            for (int x = 0; x < margin * scale; x++) {
                row[x_pos++] = 255;
            }

            // QR码内容
            for (int x = 0; x < width; x++) {
                // 模块颜色：黑色(0) 或 白色(255)
                png_byte color = (qr->data[y * width + x] & 1) ? 0 : 255;

                // 水平缩放
                for (int sx = 0; sx < scale; sx++) {
                    row[x_pos++] = color;
                }
            }

            // 右边界（白色）
            for (int x = 0; x < margin * scale; x++) {
                row[x_pos++] = 255;
            }

            png_write_row(png_ptr, row.data());
        }
    }

    // 填充下边界（白色）
    for (int y = 0; y < margin * scale; y++) {
        std::fill(row.begin(), row.end(), 255);
        png_write_row(png_ptr, row.data());
    }

    png_write_end(png_ptr, nullptr);

    // 清理资源
    png_destroy_write_struct(&png_ptr, &info_ptr);
    fclose(fp);

    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " \"text_to_encode\" \"output.png\" [scale] [margin]\n";
        std::cerr << "Example: " << argv[0] << " \"https://example.com\" qrcode.png 10 4\n";
        return 1;
    }

    const char* text = argv[1];
    const std::string filename = argv[2];
    int scale = 10;   // 默认缩放因子
    int margin = 4;   // 默认边界大小（模块数）

    if (argc > 3) scale = std::max(1, std::atoi(argv[3]));
    if (argc > 4) margin = std::max(0, std::atoi(argv[4]));

    // 生成QR码（使用MEDIUM纠错级别）
    QRcode* qr = QRcode_encodeString(text, 0, QR_ECLEVEL_M, QR_MODE_8, 1);

    if (!qr) {
        std::cerr << "Error: Failed to generate QR code\n";
        return 1;
    }

    if (saveQRCodeToPNG(filename, qr, scale, margin)) {
        std::cout << "QR code saved to " << filename
                  << " (" << (qr->width + margin * 2) * scale << "x"
                  << (qr->width + margin * 2) * scale << " pixels)\n";
    } else {
        std::cerr << "Error: Failed to save PNG file\n";
    }

    QRcode_free(qr);
    return 0;
}