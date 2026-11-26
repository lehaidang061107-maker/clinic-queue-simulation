# Nháp mục Độ phức tạp và hiệu năng (Leader, giai đoạn 5)

Ký hiệu: `N` bệnh nhân, `R` tổng số phòng, `q` độ dài hàng của một phòng, `T` số phút mô phỏng (từ 0 đến phút kết thúc ngày).

## 1. Độ phức tạp từng thao tác

| Thao tác | Thời gian | Ghi chú |
|---|---|---|
| Hoàn tất ca (bước 1 mỗi phút) | O(R) | Mỗi phòng kiểm một lần |
| Tìm người đến (bước 2) | O(1 + số người đến phút đó), khấu hao | Con trỏ `arrivalCursor_` chạy một chiều dọc kho (kho sắp theo phút đến); mỗi nút bị bước qua đúng một lần cả ngày, tổng O(N + T) |
| Chọn phòng cho một người | O(R) | Duyệt các phòng cùng khoa |
| Đưa vào FIFO / heap | O(1) / O(log q) | Heap tăng mảng thì O(q), khấu hao O(1) |
| Gọi khám từ FIFO / heap | O(1) / O(log q) | |
| Ghi hàng chờ (`observeAllQueues`, 2 lần/phút) | O(R) | |
| `isFinished()` | O(R) | So `completedCount()` với `store.size()`, rồi kiểm các phòng |
| `lastFinishMinute()` | O(N) | Gọi một lần sau khi chạy xong |
| **Cả ngày** | **O(T·R + N·(R + log q))** | |
| Bộ nhớ | O(N) | Một nút cho mỗi bệnh nhân; hàng, heap, phòng chỉ giữ con trỏ; thống kê O(R) |

**Quan hệ giữa T và N.** Giờ nhận cố định (480 phút) và thời lượng khám mỗi người cố định (5–15 phút), nên khi N lớn hàng đợi dồn lại và phút kết thúc T tăng tỉ lệ với N (T ≈ tổng thời lượng một khoa ÷ số phòng của khoa đó). Với R cố định, thời gian mô phỏng vì thế là **Θ(N)** sau tối ưu; hai phòng/khoa chỉ làm T giảm khoảng một nửa chứ không đổi bậc.

**Vì sao không phải O(log n).** Heap chỉ làm một lần gọi khám từ heap tốn O(log q). Toàn chương trình còn phải đi qua T phút, mỗi phút làm việc ở mọi phòng, và mỗi người phải được tiếp nhận, xếp hàng, gọi khám, thống kê: tổng tối thiểu là Θ(N).

## 2. Thay đổi so với giai đoạn 4

Bản giai đoạn 4 mỗi phút duyệt lại cả kho hai lần:

- `assignArrivals`: duyệt từ đầu kho để tìm người có phút đến bằng phút hiện tại;
- `isFinished`: duyệt kho kiểm mọi người đã Completed.

Mỗi lần là O(N) nên cả ngày O(T·(N + R) + N·(R + log q)) = Θ(N²) khi T ∝ N. Giai đoạn 5 thay bằng:

- **Con trỏ `arrivalCursor_`** (nút cuối cùng có phút đến nhỏ hơn phút đang xử lý). `admit()` chèn người mới với phút đến bằng phút hiện tại, luôn sau con trỏ này (ticket mới là lớn nhất nên đứng sau mọi người cùng phút), nên không bao giờ bị bỏ sót. `initialize()` đặt lại con trỏ.
- **`isFinished()` dùng bộ đếm** `Statistics::completedCount()`: mỗi người được `recordCompletion()` đúng một lần khi hoàn tất, nên "mọi người Completed" tương đương "số hoàn tất bằng số người trong kho".

## 3. Số đo trước và sau (một máy, do Leader đo)

Điều kiện: macOS 26.5 arm64, Apple clang 16.0.0, `-O2`, dữ liệu từ `generatePatients` seed 2026, giờ đóng 480, thời lượng 5–15; thời gian mô phỏng là **nhỏ nhất của 3 lần chạy**, không tính sinh dữ liệu, không in từng phút, không ghi nhật ký. Dùng chương trình đo tạm của Leader (không commit); **bảng chính thức do B đo bằng `make bench`**, số dưới đây chỉ để quyết định có tối ưu hay không.

Mô phỏng, bật ưu tiên (ms):

| N | 1 phòng/khoa trước | 1 phòng/khoa sau | 2 phòng/khoa trước | 2 phòng/khoa sau |
|---:|---:|---:|---:|---:|
| 1.000 | 8,24 | 0,44 | 2,91 | 0,38 |
| 2.000 | 32,79 | 0,65 | 16,60 | 0,63 |
| 5.000 | 170,14 | 1,04 | 84,66 | 1,04 |
| 10.000 | 729,45 | 2,32 | 375,84 | 2,34 |
| 20.000 | 2801,38 | 5,10 | 1478,57 | 4,73 |

- Trước: gấp đôi N thì thời gian tăng khoảng 3,8–4 lần (bình phương), ở cả 1 và 2 phòng/khoa.
- Sau: gấp đôi N thì thời gian tăng khoảng 2,2 lần (gần tuyến tính).
- Phút kết thúc T ở 20.000 người: 67.950 (1 phòng/khoa), 34.012 (2 phòng/khoa): T tăng theo N ở cả hai.

**Kết quả không đổi:** với 20 điểm đo (5 kích thước × 4 cấu hình), mã băm của (phút bắt đầu, mã phòng) của từng bệnh nhân giống hệt trước và sau. `results/comparison.txt` và `results/log_1phong_uutien.txt` sinh lại trùng từng byte.

## 4. Chỗ nghẽn còn lại

Sinh dữ liệu (`generatePatients` → `PatientStore::add`) vẫn tăng ×4 khi gấp đôi N: 5,6 / 12,5 / 74 / 315 / 1.248 ms cho N = 1.000 … 20.000, nghĩa là ở 20.000 người sinh dữ liệu lâu hơn mô phỏng khoảng 240 lần. Nguyên nhân: mỗi lần thêm gọi `findById`, `containsTicket` và tìm vị trí chèn, mỗi cái duyệt danh sách liên kết O(N), nên thêm N người là O(N²). Đây là phần của C; việc có sửa hay không (ví dụ chèn có con trỏ cuối khi dữ liệu đã sắp xếp) do nhóm quyết định sau khi B đo chính thức. Với bộ 10.000 người, sinh dữ liệu mất khoảng 0,3 s, chấp nhận được.
