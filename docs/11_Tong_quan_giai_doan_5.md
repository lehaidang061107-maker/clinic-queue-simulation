# TỔNG QUAN GIAI ĐOẠN 5

**Mục tiêu:** chứng minh chương trình đúng, không rò rỉ bộ nhớ, và đo được hiệu năng. Kết quả cuối gồm: bảng kiểm thử mong đợi/thực tế cho toàn bộ ca, bộ ngẫu nhiên 10.000 bản ghi, bảng và biểu đồ thời gian chạy theo kích thước dữ liệu, số liệu bộ nhớ, và nhận xét đối chiếu với độ phức tạp lý thuyết. Tất cả dùng thẳng cho báo cáo ở giai đoạn 6.

## 1. Hiện trạng đầu giai đoạn

Giai đoạn 4 đã merge vào `develop` (PR #26, #27, #30, #31). Kiểm tra lại ngày 26/11 trên macOS:

- `make` không cảnh báo; `make test-all` đạt; `make test-console` đạt 23/23 ca.
- Chạy lại menu 9 trên `patients_day.txt` ra `results/comparison.txt` trùng từng byte với bản đã commit.
- `leaks` báo 0 rò rỉ, UBSan không lỗi khi chạy so sánh 4 kịch bản.

**Lỗi tích hợp đã sửa ở mốc 0:** commit `5c0fb23` (tên "fix(a): them cau tra loi n…", chỉ định sửa 1 dòng trong `a_no_preempt.in`) đã vô tình ghi đè lại một loạt tệp. Kết quả là trên `develop` hiện **mất toàn bộ phần giai đoạn 4 của B** và phần hướng dẫn trong README:

| Bị mất | Khôi phục từ commit |
|---|---|
| `docs/Giai Doan 4/G4_B_Ban_giao.md` (tính tay bộ 5 người, bảng và nhận xét 4 kịch bản) | `1d7f762` |
| `Statistics::totalStartedCount/totalWaitingTime/averageWait/formatAverageWait` của B và 3 nhóm test chờ TB chung trong `test_b.cpp` | `1d7f762` |
| Ca `b_empty_avg`, `b_format_example_2` | `1d7f762` |
| Mục "Hướng dẫn chạy nhanh", "Tổng hợp giai đoạn 1–3", "Giải thích menu" của README | `1d7f762` |

Trước khi phát hiện, test vẫn đạt vì `develop` dùng bản `averageWait` của C nên lỗi bị che. **Leader đã khôi phục ngày 26/11** (giữ dòng sửa của `a_no_preempt.in`). Sau khôi phục trên macOS: `make test-all` đạt, `make test-console` **25/25** (thêm 2 ca của B), `comparison.txt` sinh lại trùng từng byte, `leaks` 0 rò rỉ, UBSan không lỗi.

**Việc tồn của giai đoạn 4:**

- Chưa ai chạy lại trên Windows sau khi gộp đủ B + C + A.
- Chưa ai chạy được ASan. Máy macOS của Leader dừng ngay lúc ASan khởi động (`CHECK failed: sanitizer_malloc_mac.inc`), trước khi chạy tới code nhóm: đây là lỗi toolchain, không phải lỗi code.
- A chưa chạy kiểm tra chéo hai ca `b_*` mới của B.

**Số đo nhanh có sẵn** (Leader, G4, một lần chạy, chưa lặp, tính cả thời gian sinh dữ liệu): 2.500 người 0,42 s; 5.000 người 0,97 s; 10.000 người 3,81 s (1 phòng/khoa, bật ưu tiên). Số này chỉ để định hướng, không đưa vào báo cáo.

## 2. Phương án giữ nguyên

- Mọi quy tắc nghiệp vụ đã chốt từ giai đoạn 2–4. Không đổi kết quả mô phỏng: sau mọi sửa đổi, `results/comparison.txt` sinh lại phải trùng từng byte.
- **Không đổi `DataGenerator`.** Seed 2026 phải vẫn ra đúng `patients_day.txt`. Phân bố tuổi đều 0–120 (khoảng 48% mức 1) được giữ và ghi thành giả định trong báo cáo.
- **Thời gian chờ là phút mô hình; thời gian chạy là thời gian máy.** Khi đo hiệu năng: tắt in từng phút và tắt nhật ký, tách thời gian sinh/nạp dữ liệu khỏi thời gian mô phỏng.
- Không đổi header nếu không cần; nếu cần thì báo Leader và sửa đồng bộ bên gọi trong cùng một PR.

## 3. Những việc cần hoàn thành

1. ~~Khôi phục phần G4 của B~~ — đã xong 26/11; còn A kiểm tra chéo hai ca `b_*`.
2. **Windows và ASan:** `mingw32-make`, `test-all`, `test-console` trên Windows; `make test-asan` trên Linux/WSL. Ghi máy, compiler, commit.
3. **Bộ 10.000 bản ghi `data/patients_large.txt`:** sinh bằng menu 2, seed 2026, giờ đóng 480, thời lượng 5–15. Commit tệp và ghi lại cấu hình. Kiểm tra nạp → lưu → nạp lại không mất bản ghi.
4. **Chương trình đo `tests/bench.cpp` và `make bench`:**
   - Biên dịch với `-O2` (ghi rõ trong báo cáo; bản thường không bật tối ưu).
   - Kích thước N = 1.000, 2.000, 5.000, 10.000, 20.000, sinh bằng `generatePatients` với seed 2026.
   - Cả 4 cấu hình (1/2 phòng × bật/tắt ưu tiên); mỗi điểm đo lặp 5 lần, lấy trung vị và min.
   - Đo riêng từng phần bằng `std::chrono::steady_clock`: sinh dữ liệu, mô phỏng (từ `initialize` đến `isFinished`), Merge Sort danh sách chờ, nạp và lưu tệp.
   - Ghi kèm T (phút kết thúc) để giải thích: với 1 phòng/khoa, T tăng theo N nên phần O(T·N) tăng gần bình phương.
   - Xuất `results/bench_<máy>.csv`, đầu tệp ghi máy, hệ điều hành, compiler, cờ biên dịch, commit, ngày.
5. **Bộ nhớ:**
   - Lý thuyết: `sizeof` của nút kho, nút FIFO, `Patient`; dung lượng heap sau khi tăng mảng; tổng O(N).
   - Thực tế: bộ nhớ đỉnh (macOS `/usr/bin/time -l`, Linux `/usr/bin/time -v`) cho từng N.
   - Không rò rỉ: ASan (Linux/WSL) và `leaks` (macOS) trên bộ 10.000 qua menu 9.
6. **Bảng kiểm thử tổng hợp:** mỗi ca unit test và ca menu một dòng: mục đích → dữ liệu → mong đợi → thực tế → đạt. Gom từ bàn giao G2–G5, dùng làm chương Kiểm thử của báo cáo.
7. **Cân nhắc tối ưu bước tìm người đến** (Leader): chỉ làm nếu số đo cho thấy O(T·N) chiếm ưu thế. Giữ số đo "trước" và "sau", chứng minh kết quả mô phỏng không đổi.
8. **Biểu đồ và nhận xét:** thời gian chạy theo N (có đường so với N và N²), bộ nhớ theo N, cột chờ TB theo mức × kịch bản (từ `comparison.txt`). Nhận xét dựa trên số đo, đối chiếu với bảng độ phức tạp.

**Chưa thuộc giai đoạn 5:** báo cáo hoàn chỉnh 15–30 trang, slide, bộ nộp cuối (giai đoạn 6). Bỏ về và aging vẫn không làm.

## 4. Làm theo thứ tự nào?

| Mốc | Công việc | Điều kiện để sang mốc sau |
|---|---|---|
| 0. Trả nợ G4 | ~~Leader khôi phục phần B~~ (xong); C chạy Windows và ASan; A kiểm tra chéo ca `b_*` | `test-all`, `test-console` đạt trên `develop` có đủ phần B |
| 1. Dữ liệu và công cụ đo | C commit `patients_large.txt`; B viết `bench.cpp` và `make bench`; A chuẩn bị mẫu bảng kiểm thử | `make bench` chạy hết, ra CSV đúng định dạng |
| 2. Đo | B đo đủ N × 4 cấu hình trên ít nhất 2 máy; C/A đo bộ nhớ và rò rỉ | Có CSV và số bộ nhớ cho mọi N |
| 3. Tối ưu (nếu cần) và đo lại | Leader quyết định, sửa và chứng minh kết quả không đổi; B đo lại | `comparison.txt` trùng từng byte; có số trước/sau |
| 4. Phân tích và duyệt | B vẽ biểu đồ, viết nhận xét; A hoàn thiện bảng kiểm thử; Leader duyệt | Đủ tiêu chí mục 6 |

## 5. Giai đoạn 5 gửi gì cho Leader?

- Mã: `tests/bench.cpp`, target `bench` trong Makefile, test bổ sung, sửa lỗi nếu có.
- Dữ liệu: `data/patients_large.txt`.
- Kết quả trong `results/`: `bench_<máy>.csv`, số bộ nhớ, đầu ra ASan rút gọn. Không commit `build/`.
- Tệp bàn giao trong **`docs/Giai Doan 5/`**, cùng mẫu giai đoạn 4. Biểu đồ đặt trong `docs/Giai Doan 5/img/`.

## 6. Khi nào được duyệt xong giai đoạn 5?

- `develop` có lại đủ phần G4 của B; ca `b_empty_avg`, `b_format_example_2` đạt.
- `make`, `make test-all`, `make test-console` đạt trên macOS **và** Windows; `make test-asan` đạt trên Linux/WSL. Ghi máy, compiler, commit cho từng máy.
- `data/patients_large.txt` có đúng 10.000 bản ghi, sinh lại cùng seed ra đúng tệp; nạp → lưu → nạp lại không mất bản ghi.
- ASan và `leaks` không báo lỗi trên bộ 10.000.
- `results/bench_<máy>.csv` đủ 5 kích thước × 4 cấu hình × 5 lần lặp, có thông tin máy và cờ biên dịch. Ít nhất 2 máy.
- Có biểu đồ thời gian theo N và bộ nhớ theo N; nhận xét giải thích được vì sao 1 phòng/khoa tăng gần N² còn 2 phòng/khoa gần tuyến tính, và vì sao toàn chương trình không phải O(log n).
- Nếu có tối ưu: `comparison.txt` sinh lại trùng từng byte, test bất biến trọn ngày vẫn đạt, có bảng trước/sau.
- Bảng kiểm thử tổng hợp phủ mọi ca trong `tests/` và `tests/console/`.

[Phân công giai đoạn 5](12_Phan_cong_giai_doan_5.md) · [Việc tồn chuyển sang giai đoạn 5](10_Viec_ton_giai_doan_5.md) · [Tổng quan giai đoạn 4](08_Tong_quan_giai_doan_4.md) · [Tổng quan 6 giai đoạn](../01_Tong_quan_6_giai_doan.md)
