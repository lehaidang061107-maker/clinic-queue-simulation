# PHÂN CÔNG GIAI ĐOẠN 4

**Giai đoạn này biến chương trình chạy được thành kết quả so sánh có số liệu.** Leader là bạn; A/B/C giữ phần phụ trách như trước. Mỗi người làm mốc 0 (nợ giai đoạn 3) trước, rồi mới sang việc giai đoạn 4. B chủ trì thực nghiệm và viết nhận xét.

## 1. Bắt đầu và gửi bài thế nào?

1. Đọc [tổng quan giai đoạn 4](08_Tong_quan_giai_doan_4.md), bàn giao G3 của phần mình và của phần mình phụ thuộc.
2. Kéo `develop` mới, làm trên nhánh riêng theo [quy trình Git](../MEMBER_GIT_WORKFLOW.md). Mở PR vào **`develop`** (không phải `main`).
3. Mỗi ca menu gồm `tests/console/<người>_<ca>.in` và `<người>_<ca>.expected`. `.expected` liệt kê các dòng phải xuất hiện theo thứ tự. Khi thoát, lưu vào `build/console/<ca>_working.txt`, không lưu vào `data/patients_working.txt`.
4. Chạy `make test-all` và `make test-console` trước khi mở PR; dán dòng tổng kết vào bàn giao.
5. Gửi Leader mã nguồn, test, ca menu và tệp bàn giao; ghi rõ phần chưa xong.

```text
docs/Giai Doan 4/
├── G4_Leader_Ban_giao.md
├── G4_A_Ban_giao.md
├── G4_B_Ban_giao.md
├── G4_C_Ban_giao.md
└── img/                  # Ảnh menu 9, nhật ký, sơ đồ cho báo cáo/slide

data/patients_day.txt     # Bộ trọn ngày 180 người (C)
results/
├── comparison.txt        # Bảng 4 kịch bản trên patients_day (B chạy, commit)
└── log_1phong_uutien.txt # Nhật ký mẫu (C chạy, commit)
```

**Giao diện dùng chung cần chốt sớm (mốc 1):** B thêm hàm tính chờ TB chung vào `Statistics`; Leader chốt cách lấy phút kết thúc ngày. C chỉ gọi các hàm này, không tự tính lại trong ConsoleUI. Mọi thay đổi header phải báo Leader trước khi code phần gọi.

## 2. Leader — Kiểm chứng mô phỏng và tích hợp

**Sửa:** `src/Simulation.cpp`, `include/Simulation.h` (nếu cần), `tests/test_leader.cpp`, `Makefile`, `README.md`.

**Gửi:** `G4_Leader_Ban_giao.md`, kết quả test bất biến, phần giải thích hiện tượng 2 phòng.

Mốc 0:

1. Nhắc và gom kết quả việc tồn G3: chạy Windows, kiểm tra chéo, ASan. Cập nhật mục Hiện trạng và Biên dịch trong README theo kết quả thật.
2. Duyệt PR theo thứ tự phụ thuộc: B (thống kê) → C (dữ liệu, nhật ký, menu 9) → A.

Giai đoạn 4:

1. **Phút kết thúc ngày:** chốt cách lấy (ví dụ `lastProcessedMinute()` khi `isFinished()`, hoặc thêm một hàm đọc riêng). Ghi vào header và báo B/C.
2. **Test bất biến trọn ngày** trong `tests/test_leader.cpp`: nạp `data/patients_day.txt`, chạy 4 cấu hình, rồi kiểm:
   - số bắt đầu = số hoàn tất = 180;
   - `start ≥ arrival` và `finish = start + duration`;
   - mỗi người ở phòng đúng khoa, và mỗi phòng không có hai ca chồng nhau;
   - khi tắt ưu tiên, các ca trong mỗi phòng theo thứ tự (arrival, ticket);
   - khi bật ưu tiên, không có người mức thấp hơn được gọi trong khi người mức cao hơn đã chờ sẵn ở cùng phòng.
3. **Giải thích 2 phòng bật/tắt ưu tiên trùng số liệu** (với 120 và 150 người sinh từ seed 2026):
   - Đếm số lần gọi khám mà lúc gọi, hàng chờ của phòng đó có cả người trong heap lẫn người trong FIFO.
   - Nếu số đó bằng 0 thì hai chế độ chắc chắn cho cùng kết quả: đó là giải thích.
   - Nếu khác 0 mà kết quả vẫn trùng thì có lỗi; phải tìm ra trước khi B viết nhận xét.
   - Ghi bảng số liệu này vào bàn giao.
4. **Rà menu 9 sau khi C sửa:** cả 4 kịch bản chạy trên cùng kho; giữa các kịch bản thống kê được reset; xong thì phiên trở về cấu hình cũ.
5. Viết nháp mục **Mô phỏng** cho báo cáo: thứ tự xử lý một phút, chọn phòng, khi nào kết thúc ngày, độ phức tạp một bước. Gọi N là số bệnh nhân, R là số phòng, q là độ dài hàng của một phòng; mỗi phút duyệt kho tìm người đến tốn O(N), mỗi lần gọi khám từ heap tốn O(log q). Giải thích vì sao toàn chương trình không phải O(log n).

**Ca bắt buộc (`tests/console/leader_*.in`):**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Ba người qua menu 9 | Nạp `tests/data/three_patients.txt` → menu 9 | 1 phòng bật ưu tiên chờ 0/9/3, 1 phòng tắt ưu tiên 0/4/8; 2 phòng 0/4/0 |
| Trả phiên sau so sánh | Cấu hình 2 phòng/khoa bật ưu tiên → menu 9 → chạy hết (6) | Lần chạy 6 dùng 2 phòng/khoa bật ưu tiên, không phải cấu hình của kịch bản cuối |

## 3. A — Đối chiếu nhật ký, phòng khám; sơ đồ và ảnh cho slide

**Sửa:** `tests/test_a.cpp`; `src/LinkedQueue.cpp`, `src/ClinicRoom.cpp` (chỉ khi phát hiện lỗi).

**Gửi:** `G4_A_Ban_giao.md`, ca menu phần mình, ảnh và sơ đồ trong `docs/Giai Doan 4/img/`.

Mốc 0:

1. Chạy `mingw32-make test-a` và `test-console` trên Windows, ghi vào `G3_A_Ban_giao.md` (việc tồn từ G2).
2. Kiểm tra chéo: chạy ca `c_*` của C trên máy mình, ghi kết quả vào `G3_C_Ban_giao.md`.
3. Chụp lại `a_tiep_nhan.png` (menu 4 giờ in thêm mã phòng).

Giai đoạn 4:

1. **Đối chiếu nhật ký với tính tay:** dùng nhật ký của C trên `patients_day.txt` (1 phòng/khoa, bật ưu tiên), theo dõi phòng Ngoại trong 60 phút đầu. Lập bảng: phút → ai hoàn tất → ai được gọi → số chờ heap/FIFO. So với tính tay.
2. **Hàng chờ lớn nhất:** đọc nhật ký, lấy số người chờ lớn nhất của từng phòng, so với `RoomStats.maxWaiting` trong `results/comparison.txt` (hoặc tệp `saveRunResults`). Làm ở cả 1 phòng và 2 phòng; lệch thì báo B và Leader.
3. **Không ngắt ca:** trong nhật ký, mỗi người khi đã vào khám thì giữ phòng đến đúng giờ kết thúc. Kiểm bằng một đoạn có người cấp cứu đến lúc phòng đang bận.
4. **Sơ đồ một phút** (hoàn tất → tiếp nhận/xếp phòng → gọi khám), dùng chung cho báo cáo và slide. Chụp ảnh menu 9 và một đoạn nhật ký.

**Ca bắt buộc (`tests/console/a_*.in`):** nhật ký có cấp cứu đến khi phòng bận (dùng `tests/data/three_patients.txt` hoặc bộ nhỏ tự soạn), mong đợi có dòng sự kiện gọi cấp cứu ngay sau ca hiện tại.

**Phối hợp:** lỗi thứ tự gọi do chọn phòng thì báo Leader; do so sánh heap hoặc thống kê thì báo B; do định dạng nhật ký thì báo C.

## 4. B — Chủ trì thực nghiệm: số liệu so sánh và nhận xét

**Sửa:** `src/Statistics.cpp`, `include/Statistics.h`, `tests/test_b.cpp`; `src/PriorityHeap.cpp` (chỉ khi phát hiện lỗi).

**Gửi:** `G4_B_Ban_giao.md`, `results/comparison.txt`, bảng so sánh và nhận xét.

Mốc 0:

1. Kiểm tra chéo: chạy ca `a_*` của A, ghi kết quả vào `G3_A_Ban_giao.md`.

Giai đoạn 4:

1. **Chờ TB chung toàn hệ thống:** thêm hàm vào `Statistics`, ví dụ tổng `totalWait` của 3 mức chia tổng `startedCount`; không ai bắt đầu thì báo "Chưa có dữ liệu", không chia cho 0. Thêm test vào `test_b.cpp`: bộ 3 người, bộ không có ai, bộ thiếu một mức. Báo C tên hàm trước khi C làm bảng.
2. **Tự tính tay một bảng nhỏ:** lấy `tests/data/format_example_2.txt` (5 người), tính tay cả 4 kịch bản (chờ TB/max từng mức, chờ TB chung). Đối chiếu với bảng menu 9 và ghi vào bàn giao.
3. **Chạy chính thức trên `patients_day.txt`:** menu 9, commit `results/comparison.txt`. Ghi máy, ngày, commit đã chạy.
4. **Bảng so sánh và nhận xét** (dùng cho báo cáo):
   - Bảng 4 dòng × các cột: chờ TB/max mức 0/1/2, chờ TB chung, hàng chờ lớn nhất hệ thống, phút kết thúc.
   - Nhận xét 3 ý, mỗi ý dẫn số cụ thể từ bảng: tác động của ưu tiên lên từng mức; tác động của số phòng; khi có 2 phòng thì ưu tiên còn khác biệt bao nhiêu.
   - Ghi rõ giả định: tuổi sinh đều 0–120 nên mức 1 chiếm khoảng 48%.
   - Biểu đồ cột chờ TB theo mức × kịch bản thì nên có, không bắt buộc.
5. Ghi lại chỗ thống kê còn thiếu cho giai đoạn 5 (đo thời gian chạy, bộ 10.000 người).

**Ca bắt buộc (`tests/console/b_*.in`):** menu 9 trên `format_example_2.txt` ra đúng bảng đã tính tay; chờ TB chung hiện "Chua co du lieu" khi kho rỗng.

## 5. C — Dữ liệu trọn ngày, nhật ký ra tệp và bảng menu 9

**Sửa:** `src/ConsoleUI.cpp`, `src/FileIO.cpp`, `include/FileIO.h`, `data/patients_day.txt`, `tests/test_c.cpp`, `README.md` (mục Cách chạy).

**Gửi:** `G4_C_Ban_giao.md`, dữ liệu, `results/log_1phong_uutien.txt`, ca menu phần mình.

Mốc 0:

1. Kiểm tra chéo: chạy ca `b_*` của B, ghi kết quả vào `G3_B_Ban_giao.md`.

Giai đoạn 4:

1. **`data/patients_day.txt`:** sinh bằng menu 2 (180 người, giờ đóng 480, thời lượng 5–15, seed 2026), lưu rồi commit. Không chỉnh tay. Trong bàn giao, ghi bảng phân bố: số người theo khoa, theo mức, theo từng giờ đến, tổng thời lượng mỗi khoa. Thêm vào `test_c.cpp` một kiểm tra rằng sinh lại với cùng seed ra đúng tệp này.
2. **Nhật ký ra tệp:** menu 6 hỏi thêm có ghi nhật ký không và hỏi đường dẫn (mặc định `results/log.txt`). Mỗi phút ghi:
   - dòng `Phut m`;
   - snapshot các phòng (như `printMinuteSnapshot`);
   - các dòng sự kiện: `Hoan tat: <mã> (phong p)` cho người có `finishTime == m`; `Goi kham: <mã> (phong p, cho w)` cho người có `startTime == m`.

   Lấy sự kiện bằng cách đọc trạng thái kho/phòng sau mỗi `step()`, không sửa `Simulation`. Ghi tệp lỗi thì báo rõ và vẫn chạy hết. Commit `results/log_1phong_uutien.txt` từ `patients_day.txt`.
3. **Menu 9 ra bảng:**
   - Giữ phần in chi tiết hiện có; thêm bảng gộp 1 dòng/kịch bản: chờ TB/max mức 0/1/2, chờ TB chung (hàm của B), hàng chờ lớn nhất hệ thống, phút kết thúc (cách của Leader), số bắt đầu/hoàn tất.
   - Lưu bảng vào `results/comparison.txt` (hàm mới trong FileIO); lỗi ghi thì báo rõ.
   - Xong so sánh thì `initialize` lại theo cấu hình người dùng đã chọn trước menu 9.
4. **Không đổi `DataGenerator` trong giai đoạn này**, để seed 2026 vẫn ra đúng bộ đã commit.
5. Cập nhật mục **Cách chạy** trong README: cách chạy so sánh, ghi nhật ký, đường dẫn các tệp trong `results/`.

**Ca bắt buộc (`tests/console/c_*.in`):**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Nhật ký | Nạp `three_patients.txt` → 1 phòng bật ưu tiên → menu 6, ghi nhật ký vào `build/console/c_log_results.txt` | Tệp có `Phut 5`, `Hoan tat: BN01 (phong 1)`, `Goi kham: BN03 (phong 1, cho 3)` (runner tự nối tệp `build/console/<ca>_results.txt` vào đầu ra để so) |
| Nhật ký không ghi được | Đường dẫn nhật ký trong thư mục không tồn tại | Báo lỗi, vẫn chạy hết và in thống kê |
| Bảng so sánh | Nạp `three_patients.txt` → menu 9 | Bảng 4 dòng; lưu `build/console/c_compare_results.txt` |

## 6. Kiểm tra chéo và bàn giao

Lệnh chung từ thư mục gốc:

```bash
make
make test-all
make test-console
```

Windows dùng `mingw32-make`; `test-console` cần `sh` của Git Bash hoặc MSYS2.

Mỗi tệp bàn giao G4 gồm:

1. **Đã làm gì:** tệp sửa, ứng với mục nào ở tổng quan.
2. **Cách chạy:** máy, hệ điều hành, compiler, lệnh, tệp dữ liệu, commit đã chạy.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt/chưa. Với số liệu so sánh, ghi rõ con số lấy từ tệp nào trong `results/`.
4. **Lỗi còn lại và việc chờ người khác.**

Kiểm tra chéo giai đoạn 4: **A chạy ca của B, B chạy ca của C, C chạy ca của A**, đổi vòng so với G3. Ít nhất một người chạy trên Windows. Người kiểm tra ghi kết quả vào mục kiểm tra chéo trong bàn giao của người kia.

Leader chạy lại toàn bộ test và ca menu, rà `results/comparison.txt` với nhận xét của B, rồi chỉ duyệt khi đạt [tiêu chí hoàn thành](08_Tong_quan_giai_doan_4.md#6-khi-nào-được-duyệt-xong-giai-đoạn-4).

**Chuẩn bị cho giai đoạn 5:** B giữ quy trình chạy thực nghiệm để dùng lại cho bộ 10.000 người; C chuẩn bị sinh `patients_large.txt`; A giữ sơ đồ và ảnh cho slide; Leader giữ nháp mục Mô phỏng cho báo cáo.

[Tổng quan giai đoạn 4](08_Tong_quan_giai_doan_4.md) · [Phân công giai đoạn 3](07_Phan_cong_giai_doan_3.md)
