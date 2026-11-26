# Bàn giao Giai đoạn 5 — Leader

**Phụ trách:** `include/Simulation.h`, `src/Simulation.cpp`, `tests/test_leader.cpp`, nháp [Độ phức tạp và hiệu năng](G5_Leader_Do_phuc_tap_nhap.md).

## 1. Đã làm gì

Theo mục Leader của [phân công giai đoạn 5](../12_Phan_cong_giai_doan_5.md).

| # | Việc | Trạng thái |
|---|---|---|
| 1 | Duyệt `make bench` của B | **Xong**: Leader viết hộ ở PR #37 (`-O2`, tắt in từng phút và nhật ký, tách thời gian sinh dữ liệu); số đo "trước" ở `results/bench_macos-truoc-toi-uu.csv` |
| 2 | Quyết định và cài tối ưu | **Xong** |
| 3 | Nháp Độ phức tạp và hiệu năng | **Xong**, số đo chính thức chờ B |
| 4 | Cập nhật bảng môi trường README | Dòng macOS đã cập nhật; dòng Windows và Linux/WSL **chờ C** |
| 5 | Duyệt PR theo thứ tự C → B → A → Leader | Chờ các PR |

**Tối ưu (việc 2).** Số đo trước tối ưu cho thấy mô phỏng tăng gấp 4 lần mỗi khi gấp đôi N (2,8 s ở 20.000 người, 1 phòng/khoa) vì mỗi phút duyệt lại cả kho hai lần. Đã sửa:

- `assignArrivals`: dùng con trỏ `arrivalCursor_` thay vì duyệt từ đầu kho;
- `isFinished`: so `statistics_.completedCount()` với `store_.size()` thay vì duyệt kho.

Sau tối ưu: 5,1 ms ở 20.000 người (1 phòng/khoa), gần tuyến tính theo N. Bảng trước/sau và lý do chi tiết ở [nháp](G5_Leader_Do_phuc_tap_nhap.md#3-số-đo-trước-và-sau-một-máy-do-leader-đo). Không đổi tệp header nào ngoài thêm một thành viên private `arrivalCursor_` trong `Simulation.h`.

**Test thêm vào `tests/test_leader.cpp`:**

- `admissionBeforeLaterArrivals`: tiếp nhận chen người vào giữa chừng, trước những người đã có sẵn trong kho (hai người cùng phút giữa hai lần gọi, một người sau khi con trỏ đã đi qua), rồi chạy hết và chạy lại; kiểm không ai bị bỏ sót hay xếp hai lần.
- `largeDay`: bộ 10.000 người ở cả 4 cấu hình, kiểm đủ các bất biến của ngày 180 người (đủ người, không khám trước khi đến, `finish = start + thời lượng`, đúng khoa, không chồng ca, đúng thứ tự khi tắt ưu tiên, thống kê khớp tính lại từ từng người).

## 2. Cách chạy

macOS 26.5 arm64, Apple clang 16.0.0, GNU Make 3.81, ngày 26/11/2026. Nhánh `feature/g5-leader` từ `develop` tại `08345d0`.

```bash
make
make test-all
make test-leader-isolated
make test-console
```

## 3. Kết quả

| Lệnh / kiểm tra | Mong đợi | Thực tế | Đạt |
|---|---|---|---|
| `make` | Không cảnh báo | Không cảnh báo | Đạt |
| `make test-all` | Toàn bộ PASS | 30 nhóm PASS (A 10, B 4, C 4, Leader 12) | Đạt |
| `make test-leader-isolated` | Toàn bộ PASS | 10 nhóm PASS | Đạt |
| `make test-console` | Mọi ca đạt | 25 đạt, 0 chưa đạt | Đạt |
| Lịch khám trước/sau (mã băm phút bắt đầu + phòng của từng người) | Giống hệt | Giống hệt ở 20 điểm (N = 1.000 … 20.000 × 4 cấu hình) | Đạt |
| `results/comparison.txt` sinh lại từ `patients_day.txt` | Trùng từng byte | Trùng từng byte | Đạt |
| `results/log_1phong_uutien.txt` sinh lại | Trùng từng byte | Trùng từng byte (3.256 dòng) | Đạt |
| `leaks --atExit`, menu 9 trên `patients_day.txt` | 0 rò rỉ | 0 rò rỉ | Đạt |
| UBSan, menu 9 trên `patients_day.txt` | Không lỗi | Không lỗi | Đạt |
| Đột biến thử: cho con trỏ đi quá một nút (`<` thành `<=`) | Test phải hỏng | `test-leader` hỏng ngay ở `scenarios` | Đạt |

## 4. Lỗi còn lại và việc chờ người khác

- **B:** sau khi #36 merge, chạy `make bench MACHINE=macos` để có số "sau" và so với `bench_macos-truoc-toi-uu.csv` (commit `08345d0`); vẽ biểu đồ và viết nhận xét. Số đo tạm trong nháp §3 chỉ dùng để quyết định tối ưu, không đưa vào báo cáo.
- **C:** Windows và ASan (WSL) ở mốc 0; sinh dữ liệu vẫn O(N²) trong `PatientStore::add` ([nháp §4](G5_Leader_Do_phuc_tap_nhap.md#4-chỗ-nghẽn-còn-lại)), nhóm quyết định có sửa hay không.
- **Giới hạn của tối ưu:** `arrivalCursor_` giả định kho không bị xóa hay nạp lại khi phiên đang chạy mà chưa `initialize` lại. Điều này đã đúng sẵn với phòng và hàng đợi (giữ con trỏ tới bệnh nhân). `ConsoleUI` giữ được giả định: menu 1 (nạp) và menu 2 (sinh) từ chối khi phiên đã khởi tạo, và không có đường nào xóa kho giữa phiên.
- ASan vẫn chưa chạy được trên máy macOS của Leader (lỗi toolchain lúc khởi động); thay bằng `leaks` và UBSan.
