# TỔNG QUAN GIAI ĐOẠN 2

**Mục tiêu:** chuyển thiết kế giai đoạn 1 thành bản chương trình C++17 chạy được, kiểm tra đúng các chức năng cơ bản trên dữ liệu nhỏ.

## 1. Phương án tiếp tục dùng

- Giao diện console, chạy trên Windows và macOS. Cài đúng các hàm đã khai báo trong `include/`, mã nguồn đặt trong `src/`.
- FIFO tự cài bằng danh sách liên kết; heap nhị phân tự cài trên mảng động. Không dùng container STL hoặc `sort`/`find` STL trong phần lõi; được dùng chuỗi, luồng nhập/xuất và bộ sinh ngẫu nhiên theo thiết kế.
- Có 3 khoa: Nội, Nhi, Ngoại. Chọn phòng đúng khoa có ít người chờ nhất, hòa chọn mã phòng nhỏ nhất; không tính người đang khám vào số chờ.
- Ưu tiên 0 cấp cứu → 1 dưới 6 tuổi/từ 60 tuổi → 2 thường. Cùng mức xét phút đến, rồi ticket tăng dần. Không ngắt ca đang khám.
- Bật ưu tiên: mức 0/1 vào heap, mức 2 vào FIFO; tắt ưu tiên: mọi người vào FIFO. Không đổi chế độ giữa một lần chạy.
- Mỗi phút: hoàn tất ca → nhận người đến và xếp hàng → gọi khám → snapshot. Quan sát cực đại hàng chờ cả trước và sau gọi khám theo thiết kế Leader.
- Mặc định nhận người trong phút 0–479; sinh thời lượng khám 5–15 phút. Đóng nhận vẫn khám hết người còn lại.
- Kho sở hữu Patient; hàng, heap, phòng và báo cáo chỉ mượn con trỏ. Reset trạng thái/thống kê khi chạy lại, giữ nguyên dữ liệu đầu vào để so sánh.
- Chưa bổ sung bỏ về hoặc aging; chỉ thay đổi phạm vi nếu có yêu cầu được nhóm xác nhận.

## 2. Những việc cần hoàn thành

1. Cài cấu trúc dữ liệu và các hàm kiểm tra đầu vào theo header đã có.
2. Cài phòng khám, vòng mô phỏng từng phút và thống kê; đối chiếu kết quả với ví dụ tính tay giai đoạn 1.
3. Cài đọc/ghi tệp, menu, tiếp nhận thủ công và sinh dữ liệu có seed.
4. Ghép thành chương trình có thể nạp/nhập dữ liệu, chạy từng phút hoặc chạy hết, xem thống kê, lưu và mở lại đầu vào.
5. Chạy thử cả 4 cấu hình: 1 hoặc 2 phòng mỗi khoa, có hoặc không ưu tiên, trên cùng một bộ input nhỏ. Chưa kết luận hiệu năng từ những lần chạy này.
6. Lưu mã kiểm tra, lệnh chạy và kết quả thực tế để Leader kiểm tra lại được.

## 3. Làm theo thứ tự nào?

| Mốc | Công việc | Điều kiện để ghép tiếp |
|---|---|---|
| 1. Nền tảng | Leader làm Patient/SimulationConfig; A làm LinkedQueue; B làm PriorityHeap; C làm PatientStore | Các phần biên dịch và kiểm tra độc lập được |
| 2. Xử lý chính | A làm ClinicRoom sau khi có heap; B làm Statistics/báo cáo sau khi có kho; Leader ghép Simulation | Ví dụ nhỏ đúng thứ tự khám, mốc thời gian và thống kê |
| 3. Giao tiếp | C làm FileIO, DataGenerator, ConsoleUI; Leader ghép main và lệnh build | Chạy trọn quy trình từ menu đến lưu tệp |
| 4. Duyệt | Kiểm tra chéo, chạy lại trên hai hệ điều hành và sửa lỗi tích hợp | Đủ tiêu chí ở mục 5 |

Các phần không phụ thuộc nhau có thể làm cùng lúc. Khi phần phụ thuộc chưa xong, dùng chương trình kiểm tra nhỏ riêng; không để hàm giả trả kết quả thành công trong bản bàn giao cuối.

## 4. Giai đoạn 2 gửi gì cho Leader?

- Mã nguồn `.cpp` phần mình và chương trình kiểm tra nhỏ trong `tests/`.
- Một tệp bàn giao `.md` trong **`docs/Giai Doan 2/`**, ghi chức năng đã cài, cách biên dịch/chạy, đầu vào, kết quả mong đợi và thực tế, lỗi còn lại.
- Tệp dữ liệu thử nếu có; không ghi đè dữ liệu chung mà chưa thống nhất. Dùng `data/format_example.txt` và ví dụ riêng 3 người để kiểm tra ban đầu.
- Giữ giao diện header; nếu phát hiện hợp đồng thiếu hoặc cần đổi, báo Leader để sửa đồng bộ trước khi ghép.

**Chưa bắt buộc Word, slide, báo cáo cuối hoặc đo hiệu năng 10.000 bệnh nhân.** Cần cài bộ sinh hỗ trợ số lượng theo cấu hình, nhưng việc sinh bộ lớn, chạy tải lớn và đo so sánh đầy đủ để giai đoạn sau. Không ghi số liệu “đã chạy đúng” hoặc “nhanh hơn” khi chưa thực sự chạy.

## 5. Khi nào được duyệt xong giai đoạn 2?

- Toàn bộ chương trình biên dịch và liên kết được bằng C++17; có `main`, không còn hàm thiếu cài đặt trong luồng chức năng yêu cầu. Xử lý các cảnh báo từ mã nhóm bằng `-Wall -Wextra -Wpedantic`.
- FIFO, heap, kho, phòng, thống kê và báo cáo vượt qua các ca nhỏ đã giao; reset/hủy không xóa nhầm Patient hoặc để con trỏ treo.
- Ví dụ 3 người cho đúng thời gian chờ: bật ưu tiên 0/9/3; tắt ưu tiên 0/4/8 theo thứ tự BN01/BN02/BN03.
- Từ chối dữ liệu không hợp lệ, xử lý được tệp thiếu/dòng sai và lỗi lưu; tiếp nhận phút 479 được, phút 480 bị từ chối ở cấu hình mặc định.
- Chạy hết sau giờ đóng, thống kê không cộng lặp, chạy lại giữ nguyên input. Số bắt đầu = số hoàn tất = số bệnh nhân khi kịch bản hợp lệ kết thúc.
- Lưu đầu vào có người nhập thêm rồi nạp lại không mất người; kết quả đặt riêng trong `results/`.
- Có bằng chứng chạy trên Windows và macOS; nếu mới thử một hệ điều hành phải ghi rõ còn chờ xác nhận, không ghi đã hỗ trợ đủ hai.
- Leader xem kết quả kiểm tra chéo và xác nhận các lỗi còn lại đã xử lý trước khi chuyển sang giai đoạn tiếp theo.

**Hiện trạng đầu giai đoạn:** đã có bộ thiết kế Leader/A/B/C, header, khung `.cpp` và ví dụ 5 bệnh nhân; chưa có chức năng chương trình được xác nhận chạy đúng.

[Phân công giai đoạn 2](05_Phan_cong_giai_doan_2.md) · [Tổng quan giai đoạn 1](02_Tong_quan_giai_doan_1.md) · [Thiết kế Leader](Giai%20Doan%201/G1_Leader_Thiet_ke.md)
