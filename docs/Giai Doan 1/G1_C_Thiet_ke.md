# Giai đoạn 1 — C: Lưu bệnh nhân, menu và tệp

**Phạm vi:** `PatientStore`, các hàm FileIO, ConsoleUI, DataGenerator. Thiết kế theo header hiện có, chưa cài đặt chức năng. Ví dụ và ca kiểm tra là kết quả mong đợi.

## 1. Cách làm

### 1.1. Kho bệnh nhân

`PatientStore` là **danh sách liên kết đơn**, không phải mảng động. `Node` chứa trực tiếp `Patient patient` và `Node* next`; kho giữ `head_` và `size_`. Node được chèn theo `(arrivalTime, ticketNumber)` tăng dần. Địa chỉ Patient trong node ổn định đến khi xóa kho, dù có chèn node khác.

Kho sở hữu mọi bệnh nhân: cả chưa đến, đang chờ, đang khám và hoàn tất. FIFO/heap chỉ chứa con trỏ tới những người đang chờ ở một phòng. Không chuyển quyền sở hữu khi vào/rời hàng. `clear()` và destructor xóa từng node (Patient tự được hủy cùng node); chỉ gọi khi phòng, hàng và báo cáo không còn tham chiếu.

Giao diện theo header:

- `PatientStore()` tạo kho rỗng; không có tham số capacity hoặc hàm get(index).
- `add(const PatientInput&, std::string& error)` nhận đầu vào đã validate; vẫn kiểm tra trùng mã/ticket trước khi cấp phát và chèn. Lỗi trả false, kèm lý do, không đổi kho.
- `findById` quét theo mã, trả `const Patient*` hoặc nullptr; `containsTicket` quét theo số thứ tự.
- `nextTicketNumber` tìm ticket lớn nhất rồi cộng 1, không dùng size + 1 vì ticket từ tệp có thể không liên tục. Khi đạt giới hạn số nguyên phải báo lỗi qua lớp gọi, không để tràn hay cấp ticket trùng; cách biểu diễn thất bại cần được ghi rõ khi cài đặt vì hàm chỉ trả int.
- `head()` trả `const Node*` cho UI/FileIO/thống kê chỉ đọc. Simulation có quyền cập nhật trạng thái qua friend; C không bỏ const để sửa trạng thái.
- `resetStates()` đưa tất cả Patient về NotArrived và ba trường phòng/bắt đầu/kết thúc về -1, giữ nguyên PatientInput. `size/isEmpty` đọc bộ đếm.

### 1.2. Menu và dữ liệu nhập

Các hàm là hàm tự do trong namespace clinic, không tạo lớp UI mới: `runConsole(std::istream&, std::ostream&)`, `printMinuteSnapshot`, `printStatistics`, `runToEnd`.

Menu dự kiến bên trong runConsole:

```text
1. Nạp tệp bệnh nhân (kho rỗng, trước khởi tạo)
2. Sinh bộ dữ liệu (kho rỗng)
3. Chọn cấu hình và khởi tạo/chạy lại
4. Tiếp nhận một bệnh nhân
5. Chạy một phút
6. Chạy đến hết (chọn có/không in từng phút)
7. Xem phòng và thống kê
8. Lưu dữ liệu đầu vào / lưu kết quả
9. So sánh 4 kịch bản trên cùng đầu vào
0. Lưu dữ liệu đầu vào và thoát
```

Tiếp nhận hỏi mã, họ tên, tuổi, có cấp cứu không, khoa 1–3, thời lượng nguyên dương; không hỏi ticket, arrivalTime hoặc priority dạng số. Gọi `Simulation::admit(...)`: Simulation tự cấp ticket, lấy arrivalTime = nextMinute và phân mức bằng classifyPriority. Tuổi 0–120; không cấp cứu thì dưới 6 hoặc từ 60 là mức 1, còn lại mức 2; cấp cứu luôn mức 0. Không tự chọn phòng hoặc tự thao tác hàng trong UI.

Đọc từng dòng, chuyển số và kiểm tra toàn bộ chuỗi: chữ, phần dư như `12abc`, số vượt kiểu, tuổi âm, khoa sai đều phải báo lỗi và yêu cầu nhập lại; không để lỗi stream làm lặp vô hạn. ID/tên không rỗng, không chứa dấu `|` hoặc ký tự xuống dòng để lưu đúng định dạng. EOF kết thúc nhập có kiểm soát. Từ nextMinute >= closingTime, admit từ chối nhận mới.

`printMinuteSnapshot` dùng lastProcessedMinute (trước bước đầu là -1), roomAt và getter của phòng. `printStatistics` đọc Statistics; mức chưa có ai bắt đầu in “Chưa có dữ liệu”. `runToEnd` khởi tạo từng kịch bản rồi step tới isFinished; khi chạy lại phải dùng lại cấu hình đang chọn, không reset giữa mỗi phút. Menu kiểm tra cấu hình/đầu vào và báo lỗi khởi tạo trước khi chạy. Chỉ mục “chạy hết” thực hiện khởi tạo/chạy lại theo hợp đồng runToEnd; mục “chạy một phút” dùng phiên đã initialize.

### 1.3. Tệp đầu vào và kết quả

Tệp UTF-8 phân cách bằng `|`, đúng 8 trường theo thứ tự:

```text
id|fullName|age|priority|specialtyId|ticketNumber|arrivalTime|serviceDuration
BN01|Nguyen Van An|30|2|1|1|0|5
BN02|Le Thi Binh|65|1|2|2|1|10
BN03|Tran Van Cuong|40|0|3|3|2|15
```

`loadPatients(path, store, closingTime, diagnostics)` trả `LoadResult` gồm opened/headerValid/accepted/rejected. Chỉ nạp vào kho rỗng, trước initialize. Không mở được thì opened=false, báo đường dẫn; tiêu đề sai thì headerValid=false và không nạp bản ghi. Với tệp mở/tiêu đề đúng: từng dòng dữ liệu sai bị bỏ qua, tăng rejected, in số dòng và lý do; các dòng đúng vẫn được giữ. Dòng thiếu/thừa trường, số sai, mã/ticket trùng, tuổi/mức không nhất quán hoặc arrival ngoài `[0, closingTime)` đều bị từ chối. Bỏ qua dòng trống và không tính như bản ghi; chấp nhận kết thúc dòng LF/CRLF bằng cách bỏ CR cuối dòng trước khi tách. Không âm thầm bỏ dòng dữ liệu đầu tiên như tiêu đề tùy chọn.

Đầu vào mức 0 hợp lệ với mọi tuổi; mức 1/2 phải phù hợp tuổi khi không cấp cứu. Thời lượng nhập/tệp phải nguyên dương; khoảng 5–15 là mặc định khi sinh, không phải giới hạn cứng cho mọi đầu vào.

`savePatients(path, store, error)` ghi tiêu đề và đủ 8 trường input của toàn kho, gồm cả người nhập bổ sung. Không ghi status, con trỏ, mã phòng hoặc giờ khám làm đầu vào. Khi lưu/thoát dùng `data/patients_working.txt`; mở lần sau nạp vào kho rỗng và bắt đầu phiên mới, không khôi phục giữa ca. Nếu đã có tệp working thì ưu tiên cho người dùng nạp nó để giữ người bổ sung. Lưu lỗi phải báo rõ và cho thử lại, không thông báo đã lưu thành công.

`saveRunResults(path, store, simulation, error)` ghi cấu hình, mốc thời gian, số bắt đầu/hoàn tất, thống kê từng mức/phòng/hệ thống và báo cáo chờ do B tạo. Ghi rõ là kết quả tạm nếu phiên chưa kết thúc; không tính chờ âm cho người chưa bắt đầu. Kết quả đặt trong results/, tách đường dẫn khỏi tệp input. Hai hàm lưu kiểm tra mở, ghi và kết thúc ghi; trả false kèm error nếu thất bại. Khi thiết kế triển khai nên ghi tệp tạm rồi thay tệp đích sau khi ghi thành công để tránh làm mất bản input cũ.

### 1.4. Sinh dữ liệu

Dùng `generatePatients(const GeneratorConfig&, PatientStore&, std::string& error)`; kho phải rỗng. GeneratorConfig gồm patientCount, closingTime, minServiceDuration, maxServiceDuration, seed; mặc định 30, 480, 5, 15, 2026. Chuẩn bị hai cấu hình có số bệnh nhân >=30 và >=10.000, chưa cần sinh tệp lớn ở giai đoạn 1.

Để cùng seed/cấu hình cho cùng input trên Windows và macOS, dùng bộ sinh có thuật toán xác định như `std::mt19937` và tự ánh xạ số nguyên bằng rejection sampling; không phụ thuộc srand/rand, thời gian hệ thống, hoặc cách ánh xạ của uniform_int_distribution giữa thư viện. Không dùng container STL trong lõi; bộ sinh không phải container.

Quy ước sinh: mã BN + chỉ số sinh duy nhất; tên mô phỏng theo chỉ số; tuổi 0–120; cờ cấp cứu lấy theo xác suất cố định 10%, các mức còn lại tính từ tuổi, không bốc priority độc lập. Khoa 1–3, arrival trong `[0, closingTime)`, duration trong khoảng cấu hình (mặc định 5–15). Tự Merge Sort mảng tạm theo (arrival, chỉ số sinh), sau đó cấp ticket 1..N rồi thêm vào kho. Không thay seed hoặc sinh lại giữa bốn kịch bản. Lưu qua savePatients, không thêm API saveGeneratedToFile.

## 2. Các bước xử lý và hình

```text
Kho theo (arrival,ticket):
head -> Node{Patient BN01, đến 0, ticket 1}
     -> Node{Patient BN02, đến 1, ticket 2} -> nullptr
Hàng trong phòng chỉ giữ &node.patient, không sao chép Patient.

add(input, error):
    Quét kiểm tra mã/ticket trùng; trùng thì trả false
    Tìm vị trí theo (arrival,ticket)
    Cấp phát Node chứa Patient(input, trạng thái mặc định)
    Nối node vào vị trí tìm được, tăng size, trả true
    Nếu cấp phát thất bại: không thay liên kết/bộ đếm, truyền lỗi lên

loadPatients(...):
    Kiểm tra kho rỗng và thời điểm nạp ở lớp điều phối
    Mở tệp; kiểm tra tiêu đề chính xác
    Với từng dòng dữ liệu:
        Tách đủ 8 trường, chuyển số và validatePatientInput
        Nếu lỗi: ghi số dòng/lý do, tăng rejected, tiếp tục
        Nếu store.add thất bại do trùng: ghi lý do, tăng rejected
        Ngược lại tăng accepted
    Phân biệt lỗi đọc tệp/cấp phát với dòng dữ liệu sai; báo lỗi hệ thống,
    không tiếp tục mô phỏng với dữ liệu nạp dở mà chưa thông báo

savePatients(...):
    Mở tệp đích/tạm, ghi tiêu đề
    Duyệt head() ghi 8 trường input mỗi người
    Kiểm tra trạng thái ghi/đóng trước khi báo thành công

generatePatients(...):
    Validate cấu hình, kho rỗng và giới hạn kích thước/ticket
    Tạo mảng tạm chứa input và chỉ số sinh bằng bộ RNG xác định
    Tự Merge Sort theo (arrival,chỉ số sinh); cấp ticket
    Validate rồi add từng input; dọn mảng tạm kể cả khi lỗi
```

Cấp phát thất bại không được coi như dòng nhập sai rồi bỏ qua. Với sinh mới vào kho rỗng, nếu thêm dở thất bại thì dọn dữ liệu vừa sinh và trả lỗi; chưa có phòng tham chiếu. Với nạp tệp, diagnostics phải nêu lỗi đọc/cấp phát; UI không tự chạy tiếp khi nạp chưa hoàn tất. Khi thay bộ dữ liệu phải dọn mô phỏng/báo cáo trước rồi mới clear kho.

## 3. Ví dụ và kiểm tra dự kiến

Ba dòng hợp lệ ở mục 1.3 có ticket 1/2/3, đến 0/1/2, mức phù hợp tuổi/cấp cứu và ba khoa khác nhau.

| Đầu vào/tình huống | Kết quả mong đợi |
|---|---|
| Tuổi `abc` hoặc `12abc` | “Tuổi phải là số nguyên từ 0 đến 120”; hỏi lại |
| Tuổi -1 | “Tuổi ngoài khoảng 0–120”; không thêm |
| Mã BN01 đã có | “Mã bệnh nhân BN01 đã tồn tại”; kho không đổi |
| Khoa 4 | “Chuyên khoa phải là 1, 2 hoặc 3” |
| Dòng tệp thiếu serviceDuration | Báo số dòng, “Cần đủ 8 trường”; rejected tăng |
| Tuổi 65 nhưng mức 2 | Báo mức ưu tiên không phù hợp tuổi; bỏ dòng |
| Ticket 2 bị lặp | Báo ticket trùng, không đổi người đã có |
| Arrival 479 / 480 với giờ đóng 480 | 479 hợp lệ, 480 bị từ chối |
| Thời lượng 0 / số vượt kiểu | Từ chối và giải thích, không tràn số |
| Thiếu tệp / tiêu đề sai | opened=false / headerValid=false; không thêm người |
| Một dòng sai giữa hai dòng đúng | accepted=2, rejected=1, chỉ giữ hai dòng đúng |
| Ticket trong kho là 2 và 7 | nextTicketNumber cho 8, không phải 3 |
| Chèn người đến 2 rồi đến 0 | head() duyệt người đến 0 trước; địa chỉ node cũ giữ nguyên |
| Reset phiên | Giữ input, xóa trạng thái/mốc khám |
| Nhập bổ sung rồi lưu/mở lại | Input người mới còn đủ; trạng thái phiên mới là NotArrived |
| Cùng seed/cấu hình trên hai hệ điều hành | Tệp input giống nhau, kể cả mã và ticket |
| Sinh 30 / 10.000 người | Đủ số lượng, mã/ticket duy nhất, arrival/duration đúng miền |
| Thoát gặp lỗi ghi | Báo chưa lưu được, cho sửa đường dẫn/thử lại |

Các thông báo là mẫu dự kiến, không khẳng định đã thực thi. Bốn kịch bản phải dùng cùng input; nếu nhập thêm phải chốt bộ mới và chạy lại toàn bộ so sánh.

## 4. Thời gian, bộ nhớ và so sánh

Gọi N là số người; coi độ dài mã/tên có giới hạn. Với tệp dài tùy ý cần tính thêm tổng số ký tự đọc/ghi.

| Thao tác | Thời gian | Bộ nhớ |
|---|---|---|
| findById/containsTicket/nextTicketNumber | O(N) | O(1) phụ |
| add kiểm tra trùng và tìm vị trí | O(N) | Một node mới O(1) |
| size/isEmpty/head | O(1) | O(1) |
| resetStates/clear | O(N) | O(1) phụ |
| Nạp N bản ghi bằng add | O(N²) chặn trên | O(N) kho, một dòng tạm |
| Ghi input | O(N) | O(1) phụ ngoài bộ đệm I/O |
| Sinh và sắp mảng input | O(N log(N+1)) | O(N) tạm |
| Thêm N input sinh vào kho bằng add hiện tại | O(N²), vẫn cần kiểm tra trùng/chèn | O(N) kho |
| In snapshot | O(R), R là số phòng | O(1) phụ |
| Báo cáo kết quả | O(N + m log(m+1)), m người đã bắt đầu | O(m) theo thiết kế B |

Tổng sinh rồi đưa vào kho hiện tại vẫn có chặn trên O(N²), không gọi toàn bộ là O(N log N) chỉ vì có Merge Sort. Với 10.000 người cần đo thực tế ở giai đoạn sau; nếu cần chỉ mục/tối ưu phải thống nhất header với Leader.

DSLK chèn không dời đối tượng và giữ địa chỉ ổn định, nhưng tốn con trỏ, cấp phát từng node và tìm tuyến tính. Mảng đối tượng cho truy cập chỉ số O(1) nhưng tăng mảng có thể đổi địa chỉ Patient; mảng con trỏ tránh đổi địa chỉ Patient nhưng vẫn khác header đã chốt. Giai đoạn này dùng đúng DSLK. Không dùng sort/find/container STL trong lõi.

## 5. Nguồn tham khảo và điểm tích hợp

- [Phân công](../03_Phan_cong_giai_doan_1.md), [Tổng quan](../02_Tong_quan_giai_doan_1.md).
- [PatientStore.h](../../include/PatientStore.h), [FileIO.h](../../include/FileIO.h), [ConsoleUI.h](../../include/ConsoleUI.h), [DataGenerator.h](../../include/DataGenerator.h).
- [Patient.h](../../include/Patient.h), [SimulationConfig.h](../../include/SimulationConfig.h), [Simulation.h](../../include/Simulation.h), [Tệp mẫu](../../data/format_example.txt).
- [Leader](G1_Leader_Thiet_ke.md), [A](G1_A_Thiet_ke.md), [B](G1_B_Thiet_ke.md).

C đọc A về quyền sở hữu con trỏ; B đọc C về head(), vòng đời kho và input. Các quyết định menu, dòng lỗi, seed và lưu dữ liệu ở đây thống nhất với hợp đồng header; khi cài đặt cần chốt mã lỗi tràn ticket ở lớp gọi mà không tự đổi giao diện. Tổng hợp báo cáo cuối kỳ/thực nghiệm thuộc giai đoạn sau; chưa ghi số liệu đo hoặc tuyên bố kiểm thử thành công trong giai đoạn 1.
