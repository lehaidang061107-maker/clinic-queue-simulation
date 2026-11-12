# TỔNG QUAN GIAI ĐOẠN 1

**Mục tiêu:** hoàn thành thiết kế chung trước khi cài đặt chương trình.

## 1. Phương án đã chốt

- C++17, giao diện console, chạy trên Windows và macOS.
- Tự cài FIFO bằng danh sách liên kết và heap nhị phân trên mảng cho mỗi phòng; không dùng container và sort/find STL bị cấm trong phần lõi.
- 3 khoa: Nội, Nhi, Ngoại; mặc định 1 phòng/khoa, so sánh với 2 phòng/khoa.
- Ưu tiên: cấp cứu → trẻ dưới 6 tuổi/người từ 60 tuổi → người thường. Cùng mức: đến trước khám trước, hòa thì theo số thứ tự.
- Phân vào phòng đúng khoa có ít người chờ nhất; không ngắt ca đang khám.
- Tiếp nhận 480 phút; thời lượng khám ngẫu nhiên 5–15 phút; hết giờ nhận vẫn khám hết người chờ.
- Chưa làm bỏ về và aging; bổ sung nếu giảng viên xác nhận bắt buộc.

## 2. Những việc cần hoàn thành

1. Ghi rõ cần lưu thông tin gì về bệnh nhân, phòng khám và thời gian mô phỏng.
2. Chia chương trình thành các phần; thống nhất mỗi phần nhận gì, trả gì và ai tạo/xóa dữ liệu.
3. Thiết kế menu, định dạng tệp và cách kiểm tra dữ liệu nhập.
4. Viết các bước xử lý bằng lời; nêu số bước/bộ nhớ tăng thế nào khi nhiều bệnh nhân và so sánh cách lưu khác.
5. Tự tính thứ tự khám và thời gian chờ của vài bệnh nhân, chưa cần chạy code.
6. Kiểm tra header/khung mã nguồn và chuẩn bị nội dung thiết kế cho báo cáo.

## 3. Giai đoạn 1 gửi gì cho Leader?

- Mỗi người đặt **một tệp `.md` vào `docs/Giai Doan 1/`**. Làm theo câu hỏi và ví dụ của mình trong [tệp phân công](03_Phan_cong_giai_doan_1.md); chưa hiểu chỗ nào thì ghi lại để hỏi Leader.
- Dùng và rà bộ header đã có; báo chỗ cần sửa, không tạo thiết kế riêng mâu thuẫn với nhóm.
- Leader tổng hợp để kiểm tra các phần khớp nhau trước khi viết code.

**Không bắt buộc Word, chưa nộp riêng cho thầy.** Chưa cần code thuật toán hoàn chỉnh, slide, số liệu thực nghiệm hay bộ dữ liệu lớn. Nội dung này sẽ được dùng lại trong báo cáo cuối 15–30 trang.

**Hiện có:** đặc tả, header, khung `.cpp` và ví dụ 5 bệnh nhân; đã kiểm tra cú pháp C++17. Chưa cài đặt chức năng.

[Phân công giai đoạn 1](03_Phan_cong_giai_doan_1.md) · [Quy tắc chung](../README.md#quy-tắc-chính)
