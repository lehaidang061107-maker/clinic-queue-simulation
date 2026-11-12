# TỔNG QUAN 6 GIAI ĐOẠN

**Đề tài 6: Mô phỏng hàng đợi khám bệnh** · Nhóm 4 người · C++17 · Console Windows/macOS.

## 1. Mỗi giai đoạn cần bàn giao gì?

**Bàn giao từng giai đoạn là gửi cho Leader để kiểm tra và ghép project.** Đề bài đã cung cấp không yêu cầu nộp riêng từng giai đoạn cho thầy. Đây là cách tổ chức nội bộ của nhóm.

| Giai đoạn | Thành viên gửi gì cho Leader? | Kết quả chung cần đạt |
|---|---|---|
| **1. Thiết kế** | Tệp `.md` mô tả phần mình, hình/mã giả, độ phức tạp và ví dụ; rà header dùng chung | Thống nhất dữ liệu, hàm, mô-đun, menu và định dạng tệp trước khi code |
| **2. Cấu trúc dữ liệu** | Code `.h`/`.cpp`, code kiểm tra nhỏ và tệp `.md` ghi kết quả kiểm tra phần mình | FIFO, heap, kho bệnh nhân và Merge Sort hoạt động đúng; xử lý rỗng, hòa khóa, tăng mảng và bộ nhớ |
| **3. Chức năng cơ bản** | Code chức năng/menu/đọc ghi, dữ liệu mẫu >=30 bản ghi và kết quả kiểm tra | Console tiếp nhận, cấp số, phân phòng, gọi khám được; lưu rồi mở lại không mất dữ liệu |
| **4. Mô phỏng và thống kê** | Code mô phỏng/thống kê, nhật ký chạy, tệp kết quả và bảng so sánh kịch bản | Chạy trọn ngày, in trạng thái mỗi phút, tính chờ và so sánh ưu tiên/số phòng |
| **5. Kiểm thử và thực nghiệm** | Bảng kiểm thử mong đợi/thực tế; bộ ngẫu nhiên >=10.000 bản ghi; số liệu, biểu đồ thời gian và nhận xét | Có bằng chứng chương trình đúng, không rò rỉ bộ nhớ và đánh giá được hiệu năng |
| **6. Hoàn thiện và bảo vệ** | Code đã sửa xong; nội dung báo cáo/slide của từng người; hướng dẫn chạy | Ghép thành bộ nộp cuối, chạy được trên Windows/macOS; cả nhóm giải thích được code |

Mỗi người chỉ gửi phần được giao, không phải tự làm toàn bộ sản phẩm của mỗi dòng. Leader nhận, kiểm tra và ghép các phần thành project chung. Deadline do Leader tự sắp xếp.

**Không cần 6 file Word hoặc 6 báo cáo riêng.** Tệp `.md`, ảnh, kiểm thử và kết quả chạy trong các giai đoạn là nguyên liệu cho **một báo cáo chung**. Lưu nội dung ngay khi làm, đến giai đoạn 6 tổng hợp và chỉnh hoàn thiện.

## 2. Phân công chung

| Người | Phần chính | Tài liệu và việc bổ sung |
|---|---|---|
| **Leader (bạn)** | Dữ liệu/cấu hình chung, mô phỏng, phân phòng, main và tích hợp | Viết phần mô phỏng, Giới thiệu, Kết luận; duyệt sản phẩm cuối |
| **A** | FIFO và phòng khám | Viết phần mình; hỗ trợ kiểm thử/thực nghiệm; tổng hợp slide |
| **B** | Heap, thống kê và sắp xếp | Viết phần mình; chủ trì thực nghiệm |
| **C** | Kho bệnh nhân, menu, đọc/ghi và sinh dữ liệu | Viết phần mình; dữ liệu, README và tổng hợp báo cáo |

Mỗi người bàn giao theo giai đoạn: thiết kế ở giai đoạn 1; từ giai đoạn 2 có thêm code và kiểm thử. Nội dung phần mình được giữ lại để đưa vào báo cáo chung. Người tổng hợp không viết thay cả nhóm. Thay đổi dữ liệu hoặc giao diện hàm dùng chung phải báo Leader.

## 3. Những yêu cầu phải giữ

- Mỗi phòng có **cả FIFO DSLK và heap nhị phân trên mảng**, tự cài đặt; mã chia nhiều tệp `.h`, `.cpp`.
- Không dùng `vector`, `list`, `stack`, `queue`, `map`, `set`, `unordered_map`, `priority_queue` hoặc `sort`, `find` STL cho lõi. Được dùng `string`, `iostream`, `fstream`.
- Tiếp nhận đủ mã, tên, tuổi, ưu tiên, chuyên khoa; cấp số và gọi khám theo phòng. Kiểm tra mã trùng, sai kiểu, sai miền.
- Đọc dữ liệu khi khởi động, ghi lại khi thoát; giải phóng toàn bộ bộ nhớ động.
- Thống kê chờ **trung bình/lớn nhất theo mức ưu tiên**, độ dài hàng đợi lớn nhất; so sánh **có/không ưu tiên** và **số phòng mở**.
- Console là lựa chọn chính thức. Bỏ về và aging chưa làm, chỉ bổ sung nếu giảng viên xác nhận bắt buộc.
- Ghi nguồn tham khảo và đóng góp thực tế; mọi thành viên phải hiểu mã nguồn.

## 4. Hai điểm dễ hiểu nhầm

**So sánh công bằng:** dùng cùng danh sách bệnh nhân, thời điểm đến và thời lượng khám cho các kịch bản. Chỉ thay chế độ ưu tiên hoặc số phòng; không sinh lại dữ liệu mỗi lần so sánh.

**Thời gian chờ khác thời gian chạy:** thời gian chờ là số phút trong mô hình; thời gian chạy là thời gian máy tính thực thi chương trình. Khi đo hiệu năng, tắt in từng phút và ghi rõ phần được đo. Dùng nhiều kích thước dữ liệu, ghi seed/cấu hình/máy/compiler, đo lặp và nhận xét với độ phức tạp lý thuyết. Phân tích cả thời gian và bộ nhớ từng thao tác chính; không coi toàn chương trình là O(log n) chỉ vì dùng heap.

## 5. Bộ nộp cuối cùng cho thầy

- **Mã nguồn và dữ liệu:** mẫu >=30 bản ghi, bộ ngẫu nhiên >=10.000 bản ghi.
- **Báo cáo riêng 15–30 trang:** đủ 6 phần theo Mục IV; có phân công, struct/sơ đồ mô-đun, so sánh cấu trúc dữ liệu, mã giả/lưu đồ, chạy tay, độ phức tạp, ảnh console, bảng kiểm thử, bảng/biểu đồ thực nghiệm, kết luận và tài liệu tham khảo.
- **Slide 10–15 trang:** chuẩn bị trình bày, demo và vấn đáp trong 10 phút.
- **README:** hướng dẫn build/chạy; không thay thế báo cáo.

Nếu được xác nhận thuộc nhóm B, báo cáo thêm chương Cơ sở lý thuyết sau Giới thiệu và ít nhất 3 tài liệu tham khảo, gồm bài báo gốc được gợi ý.

**Thang điểm:** cấu trúc dữ liệu/giải thuật 30%; chức năng 20%; phân tích/thực nghiệm 15%; mã nguồn 10%; báo cáo 15%; thuyết trình/vấn đáp 10%.

[Tổng quan giai đoạn 1](02_Tong_quan_giai_doan_1.md) · [Phân công giai đoạn 1](03_Phan_cong_giai_doan_1.md) · [Quy tắc chung](../README.md#quy-tắc-chính)
