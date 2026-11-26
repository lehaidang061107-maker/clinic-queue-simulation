# TỔNG QUAN GIAI ĐOẠN 1

**Mục tiêu:** thống nhất thiết kế chung trước khi viết code.

## 1. Cần làm gì?

| # | Việc | Kết quả cần có |
|---|---|---|
| 1 | Dữ liệu | Bệnh nhân, phòng khám, thời gian mô phỏng cần lưu những gì |
| 2 | Chia mô-đun | Mỗi phần nhận gì, trả gì, ai tạo và ai xóa dữ liệu |
| 3 | Menu và tệp | Danh sách mục menu, định dạng tệp, cách kiểm tra dữ liệu nhập |
| 4 | Thuật toán | Các bước xử lý bằng lời, thời gian và bộ nhớ từng thao tác, so sánh với cách lưu khác |
| 5 | Tính tay | Thứ tự khám và thời gian chờ của vài bệnh nhân |
| 6 | Header | Rà bộ header chung, báo chỗ cần sửa |

## 2. Phương án đã chốt

- C++17, console, chạy trên Windows và macOS.
- Mỗi phòng có FIFO bằng danh sách liên kết và heap nhị phân trên mảng, tự cài. Không dùng container hay `sort`/`find` STL trong phần lõi.
- 3 khoa: Nội, Nhi, Ngoại; mặc định 1 phòng/khoa, so sánh với 2 phòng/khoa.
- Ưu tiên: cấp cứu → dưới 6 tuổi hoặc từ 60 tuổi → thường. Cùng mức: đến trước khám trước, hòa thì theo số thứ tự.
- Xếp vào phòng đúng khoa có ít người chờ nhất; không ngắt ca đang khám.
- Nhận bệnh 480 phút, khám 5–15 phút; hết giờ nhận vẫn khám hết người chờ.
- Không làm bỏ về và aging.

## 3. Nộp gì?

- Mỗi người một tệp `docs/Giai Doan 1/G1_<người>_Thiet_ke.md`, làm theo [phân công](03_Phan_cong_giai_doan_1.md).
- Chưa cần code thuật toán, slide, số liệu hay bộ dữ liệu lớn.

## 4. Tiêu chí duyệt

- Đủ 4 tệp thiết kế, trả lời hết câu hỏi và có ví dụ tính tay.
- Thiết kế của 4 người khớp nhau và khớp header; không ai tự đổi quy tắc đã chốt.
- Ví dụ 3 người cho chờ 0/9/3 khi bật ưu tiên.

[Phân công giai đoạn 1](03_Phan_cong_giai_doan_1.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
