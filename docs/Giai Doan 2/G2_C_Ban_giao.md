# Bàn giao Giai đoạn 2 — Thành viên C

## 1. Chức năng đã cài đặt

- `src/PatientStore.cpp`: danh sách liên kết đơn sở hữu `Patient`; chèn theo `(arrivalTime, ticketNumber)`, từ chối trùng ID/ticket, tìm kiếm tuyến tính, cấp ticket tiếp theo có kiểm tra tràn, reset trạng thái và giải phóng kho.
- `src/FileIO.cpp`: nạp tệp vào kho rỗng; xác thực tiêu đề và đúng 8 trường; xử lý CRLF/dòng trống; kiểm tra số, dữ liệu bệnh nhân và trùng lặp theo từng dòng; tiếp tục nhận các dòng đúng khi gặp dòng sai. Có ghi input riêng và lưu kết quả phiên/thống kê/báo cáo chờ.
- `src/DataGenerator.cpp`: sinh dữ liệu bằng `std::mt19937`, ánh xạ số nguyên bằng rejection sampling, Merge Sort tự cài theo `(arrivalTime, genIndex)`, rồi cấp ticket liên tục trước khi thêm vào kho.
- `src/ConsoleUI.cpp`: menu 10 lựa chọn (0–9), đọc số nguyên toàn chuỗi có kiểm tra miền/tràn, xử lý EOF, tiếp nhận không yêu cầu ticket/phút đến, chạy từng phút/chạy hết, snapshot, thống kê, lưu và so sánh bốn kịch bản trên cùng kho input.
- `tests/test_c.cpp`: ca kiểm tra cho thứ tự/chống trùng/ticket/reset kho; nạp CRLF, dòng lỗi xen kẽ, header/tệp sai, lưu-nạp vòng lại và đường dẫn không ghi được; tính xác định, miền và thứ tự dữ liệu sinh.
- `Makefile`: thêm mục tiêu `test-c` để chạy kiểm thử C độc lập. Cờ section garbage collection loại phần ghi kết quả không được gọi (phụ thuộc mô-đun B chưa có triển khai trong nhánh này).

## 2. Cách biên dịch và chạy test

Từ thư mục gốc với GCC hoặc MinGW-w64 hỗ trợ C++17:

```text
make test-c
```

Lệnh build tương đương (thư mục `build` phải tồn tại):

```text
g++ -Iinclude -std=c++17 -Wall -Wextra -Wpedantic -ffunction-sections -fdata-sections tests/test_c.cpp src/PatientStore.cpp src/FileIO.cpp src/DataGenerator.cpp src/Patient.cpp src/SimulationConfig.cpp -Wl,--gc-sections -o build/test_c
build/test_c
```

Để build chương trình tích hợp trên MinGW-w64:

```text
g++ -Iinclude -std=c++17 -Wall -Wextra -Wpedantic src/*.cpp -o build/clinic_simulator.exe
```

Test tạo tệp tạm trong `build/` và xóa sau khi chạy. Các lệnh trên chưa được chạy trong môi trường bàn giao hiện tại: không tìm thấy `g++`, `clang++`, `cl` hoặc `make` trên PATH. Vì vậy kết quả dưới đây là phạm vi test đã viết, không phải xác nhận đã chạy đạt. Bằng chứng chạy trên Windows/macOS còn chờ môi trường có compiler.

## 3. Ca kiểm tra và kết quả

| Nhóm | Ca được đưa vào `test_c.cpp` | Kết quả hiện tại |
|---|---|---|
| PatientStore | Chèn ngược thời gian đến; hòa thời gian theo ticket; trùng ID/ticket; tra cứu; ticket 2/7 cấp 8; ticket `INT_MAX`; reset, clear và dùng lại | Đã viết; chưa chạy (thiếu compiler) |
| FileIO | CRLF; bỏ dòng rỗng; tuổi chữ/âm/tràn số; mức ưu tiên không hợp tuổi; trùng ticket/ID; khoa sai; phút đến 480; thiếu trường; giữ dòng hợp lệ hai phía dòng lỗi; header sai; tệp thiếu; kho không rỗng; lưu-nạp lại; đường dẫn không ghi được | Đã viết; chưa chạy (thiếu compiler) |
| DataGenerator | Hai kho cùng seed/cấu hình có nội dung lưu giống nhau; ticket liên tục sau sắp xếp; miền tuổi/khoa/arrival/thời lượng; kho không rỗng và cấu hình sai | Đã viết; chưa chạy (thiếu compiler) |
| Tích hợp toàn ứng dụng | Build `src/*.cpp` và chạy bốn kịch bản qua UI | Chưa thể xác nhận: `LinkedQueue.cpp`, `PriorityHeap.cpp`, `ClinicRoom.cpp` và `Statistics.cpp` trong nhánh nền còn là TODO của các phần A/B |

Lệnh kiểm tra được thử trong môi trường hiện tại:

```text
g++ ... tests\test_c.cpp ... -o build\test_c.exe
Kết quả: không chạy; g++ not found.
```

`git diff --check` đã chạy và không báo lỗi whitespace.

## 4. Độ phức tạp

Gọi `N` là số bệnh nhân, `m` là số bệnh nhân đã bắt đầu khám, `R` là số phòng; bỏ qua độ dài chuỗi và chi phí I/O theo số ký tự.

| Thao tác | Thời gian | Bộ nhớ phụ |
|---|---:|---:|
| `findById`, `containsTicket`, `nextTicketNumber` | O(N) | O(1) |
| `add` (kiểm tra trùng và tìm vị trí) | O(N) | O(1) ngoài node |
| `resetStates`, `clear` | O(N) | O(1) |
| Nạp `N` dòng hợp lệ | O(N²) chặn trên do gọi `add` từng dòng | O(N) kho và một dòng |
| Lưu input | O(N) | O(1) ngoài buffer I/O |
| Sinh và Merge Sort | O(N log N) cho sắp xếp | O(N) cho hai mảng tạm, O(log N) stack |
| Sinh và thêm vào kho | O(N²) chặn trên do `add` kiểm tra/chèn tuyến tính | O(N) kho và mảng tạm |
| Snapshot UI | O(R) | O(1) |

## 5. Điểm chờ tích hợp

- Bằng chứng biên dịch/chạy cần được bổ sung khi có GCC/MinGW-w64 hoặc compiler C++17 khác.
- Chạy test tích hợp và `saveRunResults` cần các phần A/B hoàn thành; không thay thế chúng bằng test double trong bàn giao này.
- Chưa xác nhận trực tiếp trên macOS. UI không dùng API riêng nền tảng; đường dẫn và compiler cần được kiểm tra trên môi trường đó.
