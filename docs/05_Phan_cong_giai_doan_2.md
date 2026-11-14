# PHÂN CÔNG GIAI ĐOẠN 2

**Giai đoạn này bắt đầu viết code và chạy kiểm tra thực tế.** Leader là bạn; A/B/C giữ phần phụ trách như giai đoạn 1. Mục tiêu là bản chạy được trên dữ liệu nhỏ, chưa phải sản phẩm cuối kỳ.

## 1. Bắt đầu và gửi bài thế nào?

1. Đọc [tổng quan giai đoạn 2](04_Tong_quan_giai_doan_2.md), bản thiết kế phần mình và header tương ứng.
2. Viết vào `.cpp` đã có trong `src/`, đúng namespace `clinic`, tên hàm, kiểu tham số và `const` trong header. Không chép toàn bộ code vào tệp bàn giao Markdown.
3. Viết chương trình kiểm tra nhỏ trong `tests/`, dùng `assert` hoặc so sánh đầu ra với giá trị mong đợi. Chương trình test có main riêng; không đặt main test vào `src/`.
4. Tự biên dịch và chạy phần mình, ghi lệnh và kết quả thực tế trong tệp bàn giao. Biên dịch cú pháp được chưa có nghĩa là liên kết được hoặc chức năng đúng.
5. Gửi Leader mã nguồn, test, dữ liệu thử và tệp bàn giao; ghi rõ phần chưa xong. Khi ghép gặp lệch hợp đồng, cùng sửa nguyên nhân, không tự đổi quy tắc để khớp đầu ra.

```text
docs/Giai Doan 2/             # Mỗi người tạo tệp khi có nội dung bàn giao
├── G2_Leader_Ban_giao.md
├── G2_A_Ban_giao.md
├── G2_B_Ban_giao.md
└── G2_C_Ban_giao.md

tests/                       # Tệp do từng người tự tạo
├── test_leader.cpp
├── test_a.cpp
├── test_b.cpp
└── test_c.cpp
```

Mỗi người chỉ sửa phần mình; Leader điều phối thay đổi tệp chung. Tên test là gợi ý, có thể tách nhiều tệp nhưng phải ghi cách chạy. Chưa cần tạo sẵn các tệp bàn giao rỗng. Deadline do Leader tự sắp xếp theo các mốc tích hợp ở tổng quan.

## 2. Leader — Dữ liệu chung và mô phỏng

**Đọc:** [thiết kế Leader](Giai%20Doan%201/G1_Leader_Thiet_ke.md), `Patient.h`, `SimulationConfig.h`, `Simulation.h`.

**Viết:** `src/Patient.cpp`, `src/SimulationConfig.cpp`, `src/Simulation.cpp`, `src/main.cpp`; chuẩn bị lệnh build chung.

**Gửi:** mã nguồn, kiểm tra tích hợp trong `tests/`, `G2_Leader_Ban_giao.md`.

Cần làm:

1. Cài phân loại mức ưu tiên, validate input/cấu hình, reset trạng thái và tính thời gian chờ. Kiểm tra tuổi 0–120, khoa 1–3, ticket dương, arrival trong giờ nhận, duration dương, mức phù hợp tuổi/cấp cứu và giới hạn số nguyên.
2. Cài `initialize`: kiểm tra cấu hình và toàn bộ input trước khi reset; dọn tham chiếu cũ, tạo phòng, reset kho/thống kê/đồng hồ đúng vòng đời.
3. Cài `step`: hoàn tất → nhận/xếp từng người đến theo ticket → quan sát hàng → gọi khám → quan sát/snapshot. Cùng phút phải xếp hết trước khi gọi khám; chọn phòng theo số chờ rồi mã phòng.
4. Cài `admit`: tự cấp ticket, arrival = nextMinute, kiểm tra trước khi thêm kho; từ chối khi đã đóng nhận. Simulation ghi Waiting/mã phòng; A ghi bắt đầu và hoàn tất.
5. Cài điều kiện kết thúc và các getter; không dừng khi còn hàng/ca đang khám sau giờ đóng. Trước giờ đóng, kho rỗng vẫn có thể chạy phút tiếp theo.
6. Ghép main với `runConsole`; kho phải sống lâu hơn mô phỏng. Ghi thống kê bắt đầu/hoàn tất mỗi sự kiện đúng một lần; dọn mảng báo cáo trước khi reset/xóa kho.
7. Chốt những điểm xử lý lỗi trước tích hợp: tràn nextTicketNumber, cấp phát thất bại và lỗi đọc tệp giữa chừng. Các hàm không có kiểu trả lỗi phải thống nhất cách truyền lỗi với A/B/C, không im lặng báo thành công.
8. Rà các comment TODO trỏ tới tài liệu không tồn tại; thay tham chiếu ở phần mình bằng thiết kế hiện có. Header cần đổi phải được cập nhật đồng bộ cùng bên gọi.

**Ca bắt buộc:** một khoa/một phòng, BN01 thường đến 0, BN02 thường đến 1, BN03 cấp cứu đến 2; mỗi ca 5 phút.

| Chế độ | Thứ tự khám | Bắt đầu theo thứ tự BN01/BN02/BN03 | Chờ theo thứ tự BN01/BN02/BN03 |
|---|---|---|---|
| Bật ưu tiên | BN01 → BN03 → BN02 | 0 / 10 / 5 | 0 / 9 / 3 |
| Tắt ưu tiên | BN01 → BN02 → BN03 | 0 / 5 / 10 | 0 / 4 / 8 |

Kiểm tra thêm: tuổi 5/6/59/60; khoa sai; phòng hòa số chờ; nhiều người cùng đến; cấp cứu không ngắt ca; nhận phút 479/480; hết giờ còn người chờ; chạy lại bốn cấu hình không cộng dồn. Ở ví dụ trên, hết người phút 15 nhưng với giờ đóng 480 thì phiên chưa kết thúc ngay phút 15.

## 3. A — Hàng đợi thường và phòng khám

**Đọc:** [thiết kế A](Giai%20Doan%201/G1_A_Thiet_ke.md), `LinkedQueue.h`, `ClinicRoom.h`, `Patient.h`; đối chiếu heap của B.

**Viết:** `src/LinkedQueue.cpp`, `src/ClinicRoom.cpp`.

**Gửi:** mã nguồn, `tests/test_a.cpp`, `G2_A_Ban_giao.md`.

Cần làm:

1. Cài constructor/destructor, enqueue/dequeue/peek/isEmpty/size/clear của FIFO. Lấy người cuối phải đưa cả front/rear về nullptr; clear chỉ xóa node.
2. Cài cấu hình phòng, hàng thường/ưu tiên, current và các getter. `waitingCount` cộng hai hàng, không tính current.
3. Bật ưu tiên: chỉ mức 0/1 vào heap, mức 2 vào FIFO; tắt ưu tiên: tất cả FIFO. Phòng bận không gọi thêm người; heap được lấy trước FIFO khi bật ưu tiên.
4. `startNext` cập nhật InService, phòng, giờ bắt đầu và giờ xong; `finishIfDue` chuyển Completed, trả người vừa xong và bỏ current. Không tự cộng thống kê ở A.
5. Reset dọn hàng/current, không xóa Patient và không tự sửa input. Không đổi chế độ trong khi còn người thuộc kịch bản cũ.

**Ca bắt buộc:** FIFO BN01 → BN02 → BN03, thêm BN04 rồi lấy lần lượt; kiểm tra hàng rỗng, một người, lấy hết và thêm lại. Kiểm tra phòng rỗng/bận, trước/đúng giờ hoàn tất, gọi finish hai lần không trả cùng người, hai loại hàng và ví dụ 3 người của Leader.

**Phối hợp:** làm LinkedQueue trước; ghép ClinicRoom sau khi B bàn giao heap. Chọn bệnh nhân vào phòng nào thuộc Leader. Không `delete Patient*` trong hàng hoặc phòng.

## 4. B — Heap ưu tiên, thống kê và Merge Sort

**Đọc:** [thiết kế B](Giai%20Doan%201/G1_B_Thiet_ke.md), `PriorityHeap.h`, `Statistics.h`, `PatientStore.h`.

**Viết:** `src/PriorityHeap.cpp`, `src/Statistics.cpp` (gồm hàm `makeSortedWaitReport`).

**Gửi:** mã nguồn, `tests/test_b.cpp`, `G2_B_Ban_giao.md`.

Cần làm:

1. Cài min-heap theo `(priority, arrivalTime, ticketNumber)` tăng dần; dùng đúng `comesBefore`, `grow`, `siftUp`, `siftDown` trong header.
2. Tăng sức chứa khi đầy, kiểm tra tràn kích thước; không làm mất mảng cũ khi cấp phát mới thất bại. Clear không xóa Patient; destructor chỉ xóa mảng.
3. Cài Statistics theo `recordStart`, `recordCompletion`, `observeQueues`, các getter và reset có số phòng. Cộng chờ lúc bắt đầu, không đợi Completed và không cộng lần nữa khi hoàn tất.
4. Cập nhật cực đại từng phòng và toàn hệ thống. Nhận tổng số chờ tại cùng thời điểm từ Simulation, không cộng các cực đại riêng lẻ.
5. Tạo báo cáo `const Patient**` từ người đã bắt đầu, gồm cả đang khám; Merge Sort tự cài theo chờ giảm dần, hòa ticket tăng dần. Không đổi thứ tự kho; caller chỉ delete[] mảng kết quả.
6. Cung cấp dữ liệu để UI tính trung bình theo startedCount; mức rỗng được hiển thị “Chưa có dữ liệu”.

**Ca bắt buộc:** BN01 `(1,1,1)`, BN02 `(0,2,2)`, BN03 `(1,1,3)` phải pop BN02 → BN01 → BN03. Thử rỗng, một phần tử, hòa mức/giờ đến, vượt sức chứa ban đầu và clear rồi dùng lại.

Với chờ 4 và 8 cùng mức: tổng 12, đếm 2, trung bình 6, max 8. Hai phòng lần lượt có snapshot (3,0) và (0,4): max hệ thống phải là 4. Báo cáo rỗng có count 0; người chưa bắt đầu không có trong báo cáo; reset đổi số phòng không giữ số liệu cũ.

**Phối hợp:** bàn giao heap sớm cho A; ghép báo cáo khi C có kho. Chưa đo hiệu năng hoặc kết luận heap nhanh hơn từ dữ liệu nhỏ.

## 5. C — Kho, tệp, bộ sinh và menu

**Đọc:** [thiết kế C](Giai%20Doan%201/G1_C_Thiet_ke.md), `PatientStore.h`, `FileIO.h`, `DataGenerator.h`, `ConsoleUI.h`; xem Patient/SimulationConfig/Simulation.

**Viết:** `src/PatientStore.cpp`, `src/FileIO.cpp`, `src/DataGenerator.cpp`, `src/ConsoleUI.cpp`.

**Gửi:** mã nguồn, `tests/test_c.cpp`, dữ liệu nhỏ dùng để thử, `G2_C_Ban_giao.md`.

Cần làm:

1. Kho dùng **DSLK đơn**, Node chứa trực tiếp Patient; chèn theo `(arrival,ticket)`, kiểm tra trùng mã/ticket. Cài tìm kiếm, cấp ticket, head, reset/clear đúng header; giữ địa chỉ Patient ổn định.
2. Load chỉ vào kho rỗng trước initialize; kiểm tra tiêu đề và đủ 8 trường phân cách bằng `|`. Trả LoadResult; dòng sai phải ghi số dòng/lý do và bỏ dòng đó, không bỏ cả những dòng đúng.
3. Cài savePatients và saveRunResults; input lưu riêng kết quả. Lưu input mới nhập để mở lại phiên sau. Báo lỗi mở/ghi tệp rõ ràng, không báo đã lưu khi ghi thất bại.
4. Cài generatePatients theo GeneratorConfig, validate cấu hình, mã/ticket duy nhất, arrival hợp lệ và thời lượng mặc định 5–15. Cùng seed/cấu hình phải tạo cùng input trên hai hệ điều hành; không dùng thời gian hệ thống thay seed hoặc ánh xạ ngẫu nhiên khác nhau giữa thư viện.
5. Sinh input → tự sắp `(arrival, thứ tự sinh)` → cấp ticket → thêm kho. Dùng mảng tự quản lý theo thiết kế; ghi đúng chi phí O(N²) chặn trên của khâu thêm toàn kho hiện tại.
6. Cài menu nạp/sinh, chọn cấu hình, tiếp nhận, chạy phút/chạy hết, xem, lưu và thoát theo bản thiết kế C. UI chỉ gọi Simulation, không viết lại thuật toán chọn phòng/xếp hàng.
7. Đọc số có kiểm tra toàn chuỗi và miền; xử lý nhập chữ, tràn số và EOF. Không hỏi người dùng nhập ticket/arrival của tiếp nhận thủ công; gọi admit để chương trình tự cấp.
8. In snapshot và thống kê qua các getter. `runToEnd` khởi tạo kịch bản theo hợp đồng rồi chạy hết; chạy một phút dùng step của phiên hiện tại. Khi so sánh, chốt cùng input cho bốn lần chạy.

**Ca bắt buộc:** ba dòng đúng đủ 8 trường; tuổi chữ/âm; mã/ticket trùng; khoa sai; arrival 480; thiếu trường; tiêu đề sai; thiếu tệp; dòng sai xen giữa dòng đúng; đường dẫn lưu không ghi được. Thử ticket không liên tục (2,7 → ticket tiếp theo 8), chèn ngược phút đến, reset kho, lưu/nạp lại người nhập bổ sung và sinh lại cùng seed trên dữ liệu nhỏ.

**Phối hợp:** bàn giao PatientStore trước cho Leader/B. FileIO/menu dùng validator của Leader. Không xóa kho khi phòng/báo cáo còn tham chiếu. Chưa cần sinh và commit tệp 10.000 người ở giai đoạn này.

## 6. Biên dịch, bàn giao và kiểm tra chéo

Từ thư mục gốc, sau khi đã cài đủ các phần để liên kết:

```bash
mkdir -p build
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude src/*.cpp -o build/clinic_simulator
./build/clinic_simulator
```

Windows dùng MinGW-w64, tạo thư mục build nếu chưa có, đặt đầu ra `build/clinic_simulator.exe` và chạy tệp `.exe`. Không dùng system("cls"), system("pause") hoặc API chỉ có trên Windows.

Test nhỏ biên dịch với các `.cpp` cần thiết, **không kèm `src/main.cpp`** vì test có main riêng. Ví dụ khi LinkedQueue đã cài:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude tests/test_a.cpp src/LinkedQueue.cpp -o build/test_a
./build/test_a
```

Lệnh trên chỉ phù hợp nếu test_a mới kiểm tra FIFO; khi test thêm ClinicRoom phải bổ sung ClinicRoom.cpp, PriorityHeap.cpp và các nguồn thực sự phụ thuộc. Mỗi người ghi chính xác lệnh đã chạy của mình. Không commit tệp thực thi/thư mục build. Leader có thể chạy thêm AddressSanitizer trên môi trường hỗ trợ để kiểm tra lỗi bộ nhớ, ghi rõ công cụ và kết quả thực tế.

Mỗi tệp bàn giao gồm:

1. **Đã cài gì:** danh sách hàm/chức năng, tệp nguồn và phụ thuộc.
2. **Cách chạy:** trình biên dịch, hệ điều hành, lệnh build/test và dữ liệu dùng.
3. **Kết quả kiểm tra:** bảng đầu vào → mong đợi → thực tế → đạt/chưa đạt. Không chỉ ghi “đã test OK”.
4. **Lỗi và điểm cần ghép:** lỗi còn lại, trường hợp chưa chạy, thay đổi hợp đồng được Leader thống nhất nếu có.
5. **Thời gian/bộ nhớ:** đối chiếu code thực tế với thiết kế, chưa dùng số liệu đo chưa chạy.

Kiểm tra chéo: **C chạy test A, A chạy test B, B chạy test C**; A cùng Leader đối chiếu ví dụ phòng khám. Người kiểm tra ghi lại lệnh, kết quả và lỗi phát hiện vào phần kiểm tra chéo của bàn giao; chưa chạy thì ghi chờ kiểm tra.

Leader ghép các phần, chạy ví dụ 3 người, tệp mẫu 5 người và cả bốn cấu hình; kiểm tra lưu/nạp, reset, đóng giờ nhận, quyền sở hữu bộ nhớ và build hai hệ điều hành. Chỉ duyệt khi đáp ứng [tiêu chí hoàn thành](04_Tong_quan_giai_doan_2.md#5-khi-nào-được-duyệt-xong-giai-đoạn-2).

C tiếp tục giữ vai trò tổng hợp báo cáo, A tổng hợp slide, B chủ trì thực nghiệm ở các giai đoạn sau. Giai đoạn 2 mỗi người chỉ ghi phần cài đặt/kiểm tra của mình để dùng lại cho báo cáo, chưa phải làm các sản phẩm cuối.

[Tổng quan giai đoạn 2](04_Tong_quan_giai_doan_2.md) · [Phân công giai đoạn 1](03_Phan_cong_giai_doan_1.md)
