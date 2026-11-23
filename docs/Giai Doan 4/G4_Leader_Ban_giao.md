# Bàn giao Giai đoạn 4 — Leader

**Phụ trách:** `include/Simulation.h`, `src/Simulation.cpp`, `tests/test_leader.cpp`, `Makefile`, `tests/console/leader_*`, nháp mục Mô phỏng [G4_Leader_Mo_phong_nhap.md](G4_Leader_Mo_phong_nhap.md).

## 1. Đã làm gì

Đánh số theo mục 2 của [phân công giai đoạn 4](../09_Phan_cong_giai_doan_4.md).

1. **Phút kết thúc ngày:** thêm `int Simulation::lastFinishMinute() const`. Giá trị là `finishTime` lớn nhất trong các người đã khám xong, bằng −1 nếu chưa ai xong. Hàm duyệt kho, O(N). Khi ngày đã kết thúc và có bệnh nhân, giá trị bằng `lastProcessedMinute()` (test có kiểm). **C dùng hàm này cho cột "phút kết thúc" trong bảng menu 9.**
2. **Test bất biến trọn ngày** (`fullDay` trong `tests/test_leader.cpp`):
   - Dữ liệu sinh bằng `generatePatients` với 180 người, seed 2026, cấu hình mặc định; đây đúng là cấu hình C dùng để tạo `data/patients_day.txt`. Test tự sinh dữ liệu nên không phải chờ tệp của C.
   - Chạy 4 cấu hình rồi kiểm:
     - số bắt đầu = số hoàn tất = 180, và mọi người `Completed`;
     - `start ≥ arrival`, `finish = start + duration`;
     - phòng đúng khoa, mỗi phòng không có hai ca chồng nhau;
     - tắt ưu tiên: thứ tự trong mỗi phòng theo (arrival, ticket);
     - bật ưu tiên: sau mỗi phút, không người nào được gọi khi cùng phòng còn người mức cao hơn đang chờ;
     - thống kê theo mức (số người, tổng chờ, chờ max) khớp với số tính lại từ từng bệnh nhân;
     - `lastFinishMinute() == lastProcessedMinute()`; ngày rỗng cho −1.
   - `Makefile`: `test-leader` và `test-asan` liên kết thêm `src/DataGenerator.cpp`. `test-leader-isolated` bỏ qua hai nhóm test mới vì cần mô-đun thật.
3. **Giải thích 2 phòng bật/tắt ưu tiên trùng số liệu** (`overtakeExplanation`):
   - Kết luận: **không phải lỗi**. Với 120 và 150 người, ở 2 phòng/khoa không có lần gọi nào vượt người đến trước, nên lịch hai chế độ trùng nhau từng người.
   - Test khẳng định cả hai chiều: không có vượt thì lịch trùng, lịch trùng thì không có vượt.
   - Bảng số liệu và lập luận ở [nháp Mô phỏng §4](G4_Leader_Mo_phong_nhap.md#4-vì-sao-2-phòng-bậttắt-ưu-tiên-có-lúc-cho-kết-quả-giống-hệt). **B có thể viết nhận xét dựa trên kết luận này.**
4. **Rà menu 9:**
   - `leader_compare_three`: ví dụ 3 người qua menu 9 cho đúng 0/9/3, 0/4/8 và 0/4/0. Đạt.
   - `leader_restore_after_compare`: **tái hiện được lỗi**. Cấu hình 2 phòng/khoa bật ưu tiên → menu 9 → chạy hết (6) → lưu kết quả thì tệp ghi `Priority enabled: no`, tức phiên đang ở cấu hình của kịch bản cuối. Mong đợi đã viết sẵn trong `leader_restore_after_compare.expected.pending`. **C sửa xong thì đổi tên thành `.expected`**; tới lúc đó script báo `SKIP` cho ca này.
5. **Nháp mục Mô phỏng:** [G4_Leader_Mo_phong_nhap.md](G4_Leader_Mo_phong_nhap.md) gồm thứ tự xử lý một phút, phút kết thúc ngày, bảng độ phức tạp, số đo nhanh 2.500/5.000/10.000 người và phần giải thích hiện tượng 2 phòng.

## 2. Cách chạy

macOS 26.5 arm64, Apple clang 16.0.0, GNU Make 3.81. Nhánh `feature/g4-leader`, tạo từ `develop` tại `d059466`. Ngày chạy: 23/11/2026.

```bash
make
make test-all
make test-leader-isolated
make test-console
```

## 3. Kết quả

| Lệnh / ca | Mong đợi | Thực tế | Đạt |
|---|---|---|---|
| `make` | Không cảnh báo | Không cảnh báo | Đạt |
| `make test-leader` | 10 nhóm PASS | 10 PASS và 6 dòng `INFO` (bảng bên dưới); khoảng 2,6 s CPU | Đạt |
| `make test-leader-isolated` | 9 nhóm PASS | 9 PASS (không chạy 2 nhóm cần mô-đun thật) | Đạt |
| `make test-all` | Toàn bộ PASS | Đạt | Đạt |
| `make test-console` | Mọi ca có `.expected` đạt | 18 đạt; `leader_restore_after_compare` báo `SKIP` (chờ C) | Đạt |
| `leader_compare_three` | 1 phòng bật ưu tiên: mức 1 chờ TB 3, mức 2 chờ TB 4.5 / max 9; 1 phòng tắt ưu tiên: 8 và 2 / 4; 2 phòng: 0 và 2 / 4 | Khớp | Đạt |
| `leader_restore_after_compare` | Sau menu 9, chạy 6 vẫn dùng 2 phòng/khoa bật ưu tiên | `Priority enabled: no` | **Chưa (việc của C)** |

```text
INFO n=120 rooms=1 overtakes=38 mixedCalls=30 sameSchedule=no
INFO n=120 rooms=2 overtakes=0 mixedCalls=0 sameSchedule=yes
INFO n=150 rooms=1 overtakes=267 mixedCalls=70 sameSchedule=no
INFO n=150 rooms=2 overtakes=0 mixedCalls=0 sameSchedule=yes
INFO n=180 rooms=1 overtakes=695 mixedCalls=99 sameSchedule=no
INFO n=180 rooms=2 overtakes=5 mixedCalls=6 sameSchedule=no
PASS priority on/off identical schedule iff no overtakes
```

## 4. Lỗi còn lại và việc chờ người khác

- **C:** sửa menu 9 để trả phiên về cấu hình cũ, rồi đổi tên tệp `.pending`. Dùng `lastFinishMinute()` cho cột phút kết thúc.
- **B:** dùng bảng ở nháp Mô phỏng §4 khi nhận xét về 2 phòng. Các số trong nhận xét phải lấy từ `results/comparison.txt` của `patients_day.txt`, không lấy từ bảng thử 120/150 người.
- **Hiệu năng (chuyển sang giai đoạn 5):** mỗi phút mô phỏng duyệt lại toàn bộ kho, nên cả ngày là O(T·N). Với 10.000 người, 1 phòng/khoa, chạy mất khoảng 3,8 s. Đã ghi cách giảm còn O(T·R + N·(R + log q)) ở nháp Mô phỏng §3; quyết định sau khi B đo đúng quy trình.
- **Việc tồn G3 (mốc 0)** chưa có kết quả: chạy trên Windows, kiểm tra chéo ca menu, ASan. Leader gom lại khi A/B/C gửi. README sẽ cập nhật mục Hiện trạng và Biên dịch sau khi có kết quả Windows.
