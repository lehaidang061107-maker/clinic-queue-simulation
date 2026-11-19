# PHÂN CÔNG GIAI ĐOẠN 3

**Giai đoạn này biến các mô-đun rời thành một chương trình dùng được từ menu.** Leader là bạn; A/B/C giữ phần phụ trách như trước. Mỗi người làm mốc 0 (nợ giai đoạn 2) trước, rồi mới đến việc giai đoạn 3.

## 1. Bắt đầu và gửi bài thế nào?

1. Đọc [tổng quan giai đoạn 3](06_Tong_quan_giai_doan_3.md), bàn giao G2 của phần mình và của phần mình phụ thuộc.
2. Làm việc trên nhánh riêng theo [quy trình Git](../MEMBER_GIT_WORKFLOW.md); mở PR vào `develop`, không push thẳng `main`.
3. Mỗi ca kiểm tra qua menu lưu thành một tệp nhập trong `tests/console/`, đặt tên `<người>_<ca>.in` (ví dụ `c_restart.in`) kèm tệp mong đợi `.expected` hoặc ghi mong đợi trong bàn giao.
4. Chạy: `./build/clinic_simulator < tests/console/c_restart.in > build/c_restart.out`, so với mong đợi, chép đoạn đầu ra quan trọng vào bàn giao.
5. Gửi Leader mã nguồn, test, kịch bản nhập và tệp bàn giao; ghi rõ phần chưa xong.

```text
docs/Giai Doan 3/
├── G3_Leader_Ban_giao.md
├── G3_A_Ban_giao.md
├── G3_B_Ban_giao.md
├── G3_C_Ban_giao.md
└── img/                  # Ảnh console cho báo cáo/slide

tests/
├── test_a.cpp  test_b.cpp  test_c.cpp  test_leader.cpp
└── console/              # Kịch bản nhập menu (.in) và mong đợi (.expected)
```

Mỗi ca menu phải tự chứa: bắt đầu từ trạng thái đã biết (nạp một tệp dữ liệu cố định, không dựa vào `patients_working.txt` còn sót lại từ lần chạy trước) và kết thúc bằng lựa chọn thoát.

## 2. Leader — Tích hợp và luồng phân phòng

**Đọc:** bàn giao G2 của A/B/C, `Simulation.h`, `ConsoleUI.cpp` phần tiếp nhận/chạy.

**Sửa:** `src/Simulation.cpp`, `src/main.cpp`, `Makefile`, `README.md`.

**Gửi:** `G3_Leader_Ban_giao.md`, kết quả `make test-leader` với mô-đun thật, kịch bản nhập phần mình.

Mốc 0:

1. ~~Viết `docs/Giai Doan 2/G2_Leader_Ban_giao.md`~~ — đã xong 19/11.
2. Chạy `make test-leader` (không isolated) với mô-đun thật của A/B/C, sửa lỗi ghép. Đã có `test-a`, `test-a-queue` trong `Makefile`; thêm `test-b`, `test-all`.
3. Duyệt PR của A/C cho mốc 0; chỉ chuyển sang mốc 1 khi `make` tạo được chương trình.

Giai đoạn 3:

1. Rà luồng tiếp nhận từ menu 4 đến `admit`: chương trình tự cấp ticket, arrival = phút hiện tại, xếp vào phòng ngay; menu in lại ticket, mức ưu tiên và mã phòng.
2. Kiểm tra phân phòng khi 2 phòng/khoa: hòa số chờ chọn phòng mã nhỏ; người đang khám không tính vào số chờ.
3. Chạy ví dụ 3 người qua menu ở cả hai chế độ ưu tiên; kết quả phải khớp 0/9/3 và 0/4/8.
4. Chạy `patients_sample.txt` ở 4 cấu hình, kiểm tra số bắt đầu = hoàn tất = số người; chưa cần nhận xét so sánh.
5. Build và chạy trên macOS và Windows; ghi compiler/phiên bản. Chạy AddressSanitizer trên macOS (`-fsanitize=address -g`) với test-leader và một lượt chạy hết bộ mẫu; ghi kết quả thực tế.
6. Cập nhật mục **Hiện trạng** và **Biên dịch** trong README sau khi build thật thành công.

**Ca bắt buộc (`tests/console/leader_*.in`):**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Ba người, bật ưu tiên | Nạp tệp 3 người → cấu hình 1 phòng/khoa, bật ưu tiên → chạy hết → xem | Chờ BN01/BN02/BN03 = 0/9/3 |
| Ba người, tắt ưu tiên | Như trên, tắt ưu tiên | 0/4/8 |
| Hòa phòng | 2 phòng Nội, tiếp nhận 2 người Nội cùng phút khi cả hai phòng trống | Người 1 vào phòng mã nhỏ, người 2 vào phòng còn lại |
| Cấp cứu khi bận | Phòng đang khám người thường, có 1 người thường chờ, tiếp nhận 1 cấp cứu | Cấp cứu được gọi ngay sau khi ca hiện tại xong, trước người thường chờ |
| Đóng nhận | Chạy tới phút 479 → tiếp nhận được; chạy sang 480 → tiếp nhận bị từ chối | Phút 480 báo đã đóng nhận, vẫn khám hết người chờ |

## 3. A — Gọi khám, trạng thái phòng; ảnh cho slide

**Đọc:** `G2_A_Ban_giao.md`, `ClinicRoom.h`, hàm `printMinuteSnapshot` trong ConsoleUI.

**Sửa:** `tests/test_a.cpp`, `src/LinkedQueue.cpp`, `src/ClinicRoom.cpp` (chỉ khi phát hiện lỗi).

**Gửi:** `G3_A_Ban_giao.md`, kịch bản nhập phần mình, ảnh console trong `docs/Giai Doan 3/img/`.

Mốc 0:

1. ~~Viết `tests/test_a.cpp`~~ — đã xong 19/11, `make test-a-queue` đạt. Còn lại: chạy `make test-a` đầy đủ khi có heap thật và trên máy Windows, cập nhật `G2_A_Ban_giao.md`.

Giai đoạn 3:

1. Kiểm tra màn hình snapshot (menu 5, 7): mỗi phòng hiện đúng người đang khám, giờ xong, số chờ heap và FIFO; phòng trống hiện rõ là trống.
2. Theo dõi một phòng qua từng phút trên bộ mẫu đoạn đầu (khoảng 20 phút đầu): đối chiếu thứ tự gọi khám với tính tay. Ghi bảng phút → người hoàn tất → người được gọi.
3. Kiểm tra tắt ưu tiên: heap luôn rỗng, mọi người qua FIFO đúng thứ tự đến.
4. Chụp ảnh console các luồng chính (menu, tiếp nhận, snapshot, thống kê, lưu/thoát) cho báo cáo và slide; đặt tên ảnh theo luồng.

**Ca bắt buộc (`tests/console/a_*.in`):** snapshot phòng trống; phòng bận có cả heap và FIFO; tắt ưu tiên; theo dõi 20 phút đầu bộ mẫu.

**Phối hợp:** lỗi thứ tự gọi khám do chọn phòng thì báo Leader; do so sánh heap thì báo B. Không sửa phần của người khác.

## 4. B — Thống kê và báo cáo chờ trên menu

**Đọc:** `G2_B_Ban_giao.md`, `Statistics.h`, hàm `printStatistics` trong ConsoleUI, `saveRunResults` trong FileIO.

**Sửa:** `src/PriorityHeap.cpp`, `src/Statistics.cpp`, `tests/test_b.cpp` (chỉ khi phát hiện lỗi).

**Gửi:** `G3_B_Ban_giao.md`, kịch bản nhập phần mình.

B không có việc tồn đọng ở mốc 0, nên làm luôn phần giai đoạn 3 và hỗ trợ Leader khi ghép gặp lỗi liên quan heap/thống kê.

Giai đoạn 3:

1. Kiểm tra màn thống kê (menu 7) sau khi chạy hết bộ mẫu: mỗi mức ưu tiên có số người, chờ trung bình, chờ lớn nhất; mức không có ai hiện “Chưa có dữ liệu”.
2. Kiểm tra danh sách chờ sắp giảm dần, hòa theo ticket tăng dần; người chưa bắt đầu không xuất hiện; chạy giữa chừng thì người đang khám có trong danh sách.
3. Chạy lại cùng kịch bản hai lần liên tiếp (menu 3 rồi 6, lặp lại): thống kê lần hai giống lần một, không cộng dồn.
4. Kiểm tra nội dung `results/run_results.txt` khớp màn thống kê.

**Ca bắt buộc (`tests/console/b_*.in`):** thống kê bộ 3 người hai chế độ; chạy lại hai lần; xem thống kê giữa chừng; bộ mẫu không có người mức 0 (nếu C cung cấp tệp con) hiện “Chưa có dữ liệu”.

## 5. C — Khởi động/thoát, dữ liệu mẫu và menu

**Đọc:** `G2_C_Ban_giao.md`, `ConsoleUI.cpp`, `FileIO.h`, `DataGenerator.h`.

**Sửa:** `src/ConsoleUI.cpp`, `src/FileIO.cpp`, `data/patients_sample.txt`, `tests/test_c.cpp`.

**Gửi:** `G3_C_Ban_giao.md`, dữ liệu mẫu, kịch bản nhập phần mình.

Mốc 0:

1. ~~Chạy `make test-c` thật~~ — đã đạt trên macOS 19/11. Còn lại: cài MinGW-w64, chạy `mingw32-make test-c` trên Windows và bổ sung vào `G2_C_Ban_giao.md`.

Giai đoạn 3:

1. **Nạp khi khởi động:** đầu `runConsole`, nếu có `data/patients_working.txt` thì nạp và in số người nhận/từ chối; không có tệp thì báo “chưa có dữ liệu phiên trước” và chạy tiếp với kho rỗng. Không cần đổi header.
2. **Lưu khi thoát:** chọn 0 lưu vào `patients_working.txt`; lưu thất bại thì hỏi lại, không thoát mất dữ liệu. EOF giữa chừng không làm treo chương trình.
3. **Dữ liệu mẫu ≥30 bản ghi** trong `data/patients_sample.txt`, tự soạn có chủ đích (có thể sinh bằng menu 2 rồi chỉnh tay), gồm ít nhất: đủ 3 khoa; đủ mức 0/1/2; tuổi 0, 5, 6, 59, 60, 120; ≥3 người đến cùng một phút; một người đến phút 479; thời lượng 5–15. Ghi bảng phủ ca vào bàn giao.
4. Thêm tệp dữ liệu lỗi `tests/data/invalid_mix.txt` (dòng đúng xen dòng sai) dùng cho ca nạp.
5. Rà các thông báo menu: nhập sai thì nói sai gì và cho nhập lại; sau tiếp nhận in ticket, mức ưu tiên, phòng.
6. Viết nháp mục **Cách chạy** cho README: build, chạy, menu, đường dẫn dữ liệu/kết quả. Leader duyệt và ghép.

**Ca bắt buộc (`tests/console/c_*.in`):**

| Ca | Thao tác | Mong đợi |
|---|---|---|
| Lần đầu chạy | Không có `patients_working.txt` → thoát | Báo chưa có dữ liệu, kho rỗng, thoát bình thường |
| Lưu/mở lại | Nạp mẫu → tiếp nhận 2 người → thoát → chạy lại | Lần sau nạp đủ số người, 2 người mới có ticket tiếp nối |
| Nhập sai | Tuổi “abc”, tuổi −1, khoa 4, mã trùng | Từng lỗi bị từ chối, cho nhập lại, không thoát |
| Tệp lỗi | Nạp `invalid_mix.txt` | Nhận các dòng đúng, báo số dòng và lý do dòng sai |
| Không ghi được | Lưu vào thư mục không tồn tại | Báo lỗi, không báo “đã lưu” |
| EOF | Tệp nhập dừng giữa chừng | Chương trình kết thúc, không lặp vô hạn |

**Phối hợp:** đường dẫn được tính từ thư mục gốc project; ghi rõ trong README phải chạy từ thư mục gốc. Chưa cần sinh bộ 10.000 người.

## 6. Kiểm tra chéo và bàn giao

Lệnh chung từ thư mục gốc:

```bash
make                # build/clinic_simulator
make test-all       # test-a, test-b, test-c, test-leader
./build/clinic_simulator < tests/console/leader_three_priority.in
```

Windows dùng MinGW-w64 (`mingw32-make`) và chạy `build\clinic_simulator.exe < tests\console\...`.

Mỗi tệp bàn giao G3 gồm:

1. **Đã làm gì:** chức năng/tệp sửa, liên quan tới mục nào ở tổng quan.
2. **Cách chạy:** máy, hệ điều hành, compiler, lệnh build/test, tệp nhập dùng.
3. **Kết quả:** bảng ca → mong đợi → thực tế → đạt/chưa. Kèm đoạn đầu ra hoặc ảnh cho ca quan trọng. Không ghi “đã test OK” chung chung.
4. **Lỗi còn lại và việc chờ người khác.**

Kiểm tra chéo: **A chạy kịch bản của C, B chạy kịch bản của A, C chạy kịch bản của B**, mỗi người dùng máy của mình; ít nhất một người chạy trên Windows. Người kiểm tra ghi kết quả vào mục kiểm tra chéo của bàn giao người kia.

Leader chạy lại toàn bộ `tests/console/`, kiểm tra lưu/mở lại và ASan, rồi chỉ duyệt khi đạt [tiêu chí hoàn thành](06_Tong_quan_giai_doan_3.md#6-khi-nào-được-duyệt-xong-giai-đoạn-3).

**Chuẩn bị cho giai đoạn sau:** C giữ các bảng kiểm tra cho báo cáo; A giữ ảnh console cho slide; B ghi lại các chỗ thống kê cần thêm cho so sánh 4 kịch bản ở giai đoạn 4.

[Tổng quan giai đoạn 3](06_Tong_quan_giai_doan_3.md) · [Phân công giai đoạn 2](05_Phan_cong_giai_doan_2.md)
