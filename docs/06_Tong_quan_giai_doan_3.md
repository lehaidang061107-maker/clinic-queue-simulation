# TỔNG QUAN GIAI ĐOẠN 3

**Mục tiêu:** chương trình console dùng được ở mức chức năng cơ bản: tiếp nhận, cấp số, phân phòng, gọi khám; nạp dữ liệu khi khởi động, lưu khi thoát, mở lại không mất người.

## 1. Cần làm gì?

| # | Việc | Kết quả cần có |
|---|---|---|
| 1 | Ghép mô-đun thật | `make` tạo `build/clinic_simulator`, không cảnh báo với `-Wall -Wextra -Wpedantic` |
| 2 | Khởi động và thoát | Tự nạp `data/patients_working.txt` nếu có; chọn 0 thì lưu rồi thoát |
| 3 | Tiếp nhận thủ công | Nhập mã, tên, tuổi, cấp cứu, khoa; chương trình tự cấp ticket, phút đến, mức, phòng và in lại |
| 4 | Gọi khám và xem | Chạy từng phút hoặc chạy hết; xem từng phòng; xem danh sách chờ sắp bằng Merge Sort |
| 5 | Dữ liệu mẫu | `data/patients_sample.txt` ≥30 bản ghi: đủ khoa, đủ mức, tuổi biên, nhiều người cùng phút |
| 6 | Kịch bản menu | Mỗi ca là tệp `tests/console/<người>_<ca>.in` kèm `.expected`, chạy bằng `make test-console` |

## 2. Quy tắc

- Giữ mọi quy tắc đã chốt ở giai đoạn 2.
- Không đổi header nếu không cần; nếu đổi thì báo Leader và sửa cả bên gọi trong cùng PR.
- `ConsoleUI` chỉ gọi `Simulation`/`FileIO`/`PatientStore`, không tự xếp hàng hay chọn phòng.

## 3. Thứ tự làm

| Mốc | Việc | Xong khi |
|---|---|---|
| 0 | Chạy `test-a`, `test-leader` với mô-đun thật; build trên Windows | `make` tạo được chương trình, 4 test đạt |
| 1 | C: nạp khi khởi động, bộ mẫu; Leader: rà tiếp nhận và phân phòng; B: nối báo cáo chờ vào menu | Nạp mẫu → tiếp nhận → chạy → xem → lưu → thoát chạy trọn |
| 2 | Mỗi người viết, chạy và lưu kịch bản menu phần mình | Các ca ở phân công có kết quả thực tế |
| 3 | Kiểm tra chéo, chạy Windows và macOS | Đủ tiêu chí mục 5 |

## 4. Nộp gì?

- Mã, test trong `tests/`, kịch bản trong `tests/console/`.
- Bàn giao `docs/Giai Doan 3/G3_<người>_Ban_giao.md`; ảnh console trong `docs/Giai Doan 3/img/`.
- Không commit `build/` hay tệp kết quả sinh ra khi chạy test.

## 5. Tiêu chí duyệt

- `make` và `make test-all` đạt trên macOS và Windows (máy nào chưa thử phải ghi rõ).
- Khởi động có hoặc không có `patients_working.txt` đều chạy được; tệp có dòng sai vẫn nạp các dòng đúng và báo số dòng lỗi.
- Nạp `patients_sample.txt` và chạy hết: số bắt đầu = số hoàn tất = số bệnh nhân; không ai bắt đầu trước phút đến.
- Tiếp nhận: ticket tự tăng; mã trùng, tuổi sai, khoa sai, nhập chữ bị từ chối và cho nhập lại; phút 480 bị từ chối.
- Người mới vào đúng khoa, đúng phòng ít người chờ nhất; cấp cứu được gọi trước người thường nhưng không ngắt ca.
- Lưu → thoát → mở lại: số người, mã, ticket, phút đến, thời lượng giữ nguyên.
- Lỗi ghi tệp báo rõ, không báo "đã lưu".
- Ví dụ 3 người qua menu: chờ 0/9/3 (bật ưu tiên), 0/4/8 (tắt).

[Phân công giai đoạn 3](07_Phan_cong_giai_doan_3.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
