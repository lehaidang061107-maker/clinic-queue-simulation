# Giai đoạn 1 — Leader: Cách cả chương trình hoạt động

**Phạm vi:** `Patient`, `SimulationConfig`, `Simulation`, `main` và thiết kế tích hợp A/B/C.

Bản bàn giao này mô tả thiết kế, mã giả, ví dụ tính tay và kiểm tra dự kiến. Chưa cài đặt thuật toán, chưa có kết quả kiểm thử thực thi. Giữ nguyên các header hiện có.

## 1. Cách làm

### 1.1. Một bệnh nhân cần lưu gì?

`Patient` gồm dữ liệu đầu vào `PatientInput` và trạng thái thay đổi trong mô phỏng.

| Trường | Ý nghĩa và thời điểm có dữ liệu |
|---|---|
| `input.id`, `input.fullName` | Mã duy nhất và họ tên, có khi nhập/đọc tệp |
| `input.age` | Tuổi 0–120, có khi tiếp nhận |
| `input.priority` | 0 cấp cứu; 1 dưới 6 hoặc từ 60 tuổi nếu không cấp cứu; 2 thông thường |
| `input.specialtyId` | 1 Nội, 2 Nhi, 3 Ngoại; nhập riêng, không suy ra từ tuổi |
| `input.ticketNumber` | Số thứ tự nguyên dương, duy nhất; chương trình cấp khi nhập thủ công, kiểm tra khi đọc tệp |
| `input.arrivalTime` | Phút đến trong `[0, closingTime)`; nhập thủ công dùng `nextMinute()` |
| `input.serviceDuration` | Số phút khám nguyên dương, có sẵn trước khi chạy để các kịch bản dùng cùng dữ liệu |
| `status` | Ban đầu `NotArrived`, sau đó `Waiting → InService → Completed` |
| `assignedRoomId` | Ban đầu -1; có khi được xếp vào phòng, giữ nguyên đến hết kịch bản |
| `startTime` | Ban đầu -1; có khi bắt đầu khám |
| `finishTime` | Ban đầu -1; khi bắt đầu khám đặt bằng `startTime + serviceDuration` |

`finishTime` là mốc kết thúc được xác định ngay lúc bắt đầu, còn trạng thái chỉ chuyển `Completed` khi mô phỏng xử lý tới mốc đó. Thời gian chờ chỉ tính khi `startTime >= 0`: `startTime - arrivalTime`.

`classifyPriority(age, emergency)` xét cấp cứu trước, sau đó xét tuổi. Dữ liệu nhập phải được kiểm tra kiểu, miền, mã/số trùng và tính nhất quán của mức ưu tiên với tuổi. Không đổi `PatientInput` trong khi chạy một kịch bản.

### 1.2. Cấu hình và vòng đời chương trình

`SimulationConfig` mặc định tiếp nhận 480 phút, mỗi khoa một phòng, bật ưu tiên. Giờ nhận là phút 0–479; phút 480 ngừng nhận nhưng vẫn khám tiếp. So sánh bốn cấu hình: 1 hoặc 2 phòng mỗi khoa, kết hợp bật hoặc tắt ưu tiên.

`main` tạo kho bệnh nhân trước đối tượng mô phỏng để kho sống lâu hơn các con trỏ tham chiếu nó. C đọc dữ liệu và cung cấp menu; menu gọi chức năng của mô phỏng thay vì tự chọn phòng hoặc tự gọi bệnh nhân.

`initialize(config, error)` kiểm tra cấu hình và toàn bộ đầu vào trước khi reset hoặc tạo phòng. Có lỗi thì trả `false`, kèm thông báo, không xóa trạng thái cũ chỉ vì dữ liệu không hợp lệ. Cần kiểm tra cộng thời điểm khám/kích thước phòng không tràn kiểu số khi cài đặt. Việc cấp phát phải bảo đảm không rò bộ nhớ khi thất bại.

### 1.3. Một phút làm gì trước, làm gì sau?

Tại phút `t = nextMinute()`:

1. **Hoàn tất ca:** duyệt các phòng, gọi `finishIfDue(t)`; mỗi ca vừa xong tăng số hoàn tất một lần.
2. **Nhận người đến:** duyệt các bệnh nhân `NotArrived` có `arrivalTime == t`, theo số thứ tự tăng dần khi đến cùng phút.
3. **Xếp hàng:** với từng người vừa nhận, chọn phòng đúng khoa rồi đưa vào hàng ngay, trước khi chọn phòng cho người tiếp theo. Ghi phòng được phân và trạng thái `Waiting`.
4. **Gọi khám:** sau khi xếp hết lượt đến, từng phòng rảnh gọi `startNext(t, usePriority)`. Nếu trả về bệnh nhân thì ghi thống kê bắt đầu một lần.
5. **Ghi trạng thái:** quan sát hàng chờ, cung cấp snapshot cuối phút cho UI; tăng `nextMinute` lên `t + 1`.

Khi bật ưu tiên, mức 0/1 vào heap, mức 2 vào FIFO. Phòng rảnh lấy heap trước FIFO; trong heap so sánh mức ưu tiên → phút đến → số thứ tự tăng dần. Tắt ưu tiên thì tất cả vào FIFO. Không ngắt ca đang khám dù cấp cứu mới đến.

Quy ước thiết kế cho cực đại hàng chờ: quan sát **sau xếp hàng, trước gọi khám**, và ở snapshot cuối phút. Cách này bắt được số người chờ trước khi có người được gọi; UI vẫn hiển thị snapshot sau gọi khám. Số chờ không gồm người đang khám. Tổng hệ thống được cộng từ tất cả phòng tại cùng thời điểm, không cộng các cực đại riêng lẻ.

### 1.4. Có nhiều phòng cùng khoa thì chọn thế nào?

`selectRoom(specialtyId)` chỉ xét phòng đúng khoa. Chọn `waitingCount()` nhỏ nhất; hòa thì chọn mã phòng nhỏ nhất. Số chờ bằng số phần tử FIFO cộng heap, không tính người đang khám. Không dùng tình trạng rảnh/bận làm khóa phụ vì đặc tả chỉ quy định số chờ và mã phòng. Không chuyển phòng sau khi đã xếp hàng.

Ví dụ: phòng 1 đang khám và có 0 người chờ; phòng 2 rảnh và có 0 người chờ. Người mới vẫn vào phòng 1 vì hòa số chờ và phòng 1 có mã nhỏ hơn. Đây là hệ quả của quy tắc đã chốt. Nếu hai người cùng đến khi cả hai hàng đều rỗng, người có ticket nhỏ hơn vào phòng 1; sau khi cập nhật hàng, người tiếp theo vào phòng 2.

### 1.5. Khi nào kết thúc?

`isFinished()` chỉ đúng khi đã đóng tiếp nhận và không còn người chưa đến, đang chờ hoặc đang khám. Hết giờ nhận không xóa hàng và không dừng ca đang khám. Phải tiếp tục xử lý đến khi tất cả hoàn tất.

Trước giờ đóng, kể cả hệ thống đang rỗng, vẫn có thể xử lý các phút tiếp theo và nhận người thủ công. Dữ liệu đã kiểm tra phải có thời điểm đến trước giờ đóng nên không tồn tại người hợp lệ còn chờ đến sau giờ đóng. `step()` trả `false` nếu chưa khởi tạo hoặc đã kết thúc; `lastProcessedMinute()` bằng -1 trước bước đầu.

`admit(...)` chỉ được gọi trước bước đầu hoặc giữa hai bước. Hàm dùng `arrivalTime = nextMinute()`, tự cấp ticket, kiểm tra trước khi thêm vào kho. Từ phút `closingTime` trở đi phải từ chối tiếp nhận. Thêm thành công vào kho chưa có nghĩa là đã được khám: bệnh nhân được xử lý trong bước tương ứng phút đến.

### 1.6. Chạy lại cần xóa gì, giữ gì?

Trước khi chạy cấu hình mới:

1. Dùng xong và giải phóng mảng báo cáo mượn con trỏ của kịch bản cũ.
2. Reset các phòng: dọn node FIFO, làm rỗng heap, bỏ con trỏ người đang khám; giải phóng mảng phòng cũ nếu tạo lại.
3. Gọi `PatientStore::resetStates()`: mọi bệnh nhân về `NotArrived`, `assignedRoomId/startTime/finishTime = -1`.
4. Reset số người bắt đầu/hoàn tất, tổng và cực đại chờ, cực đại hàng đợi; tạo lại thống kê theo số phòng mới.
5. Đặt đồng hồ về 0, áp dụng cấu hình mới và chạy lại.

Giữ nguyên mã, tên, tuổi, mức ưu tiên gốc, khoa, số thứ tự, phút đến và thời lượng khám. Không sinh lại dữ liệu giữa các kịch bản; tắt ưu tiên chỉ đổi cách xếp hàng, không đổi trường ưu tiên của bệnh nhân. Nếu có nhập bổ sung giữa một lần chạy, phải chốt lại một bộ đầu vào chung rồi chạy lại cả bốn kịch bản để so sánh công bằng.

### 1.7. Ai sở hữu và giải phóng dữ liệu?

| Thành phần | Sở hữu | Trách nhiệm giải phóng |
|---|---|---|
| `PatientStore` | Node và `Patient` trong mỗi node | Xóa khi `clear()`/hủy kho, chỉ sau khi các tham chiếu đã được dọn |
| `LinkedQueue` | Node hàng đợi, mỗi node mượn `Patient*` | Xóa node, không xóa bệnh nhân |
| `PriorityHeap` | Mảng `Patient*` | Xóa mảng, không xóa bệnh nhân |
| `ClinicRoom` | Hai hàng đợi; mượn con trỏ người đang khám | Reset hàng và bỏ con trỏ hiện tại |
| `Simulation` | Mảng phòng; giữ tham chiếu đến kho | Dọn phòng và tài nguyên mô phỏng trước khi kho bị hủy |
| `Statistics` | Mảng thống kê phòng | Giải phóng khi reset/hủy |
| Mảng báo cáo chờ | Mảng con trỏ đọc bệnh nhân | Bên gọi `delete[]` mảng, không xóa bệnh nhân |

Kho giữ địa chỉ bệnh nhân ổn định cho tới `clear()`. Khi thoát: lưu đầu vào qua FileIO, giải phóng báo cáo và mô phỏng, cuối cùng hủy kho. Kết quả mô phỏng được lưu riêng trong `results/`, không ghi đè dữ liệu đầu vào.

## 2. Các bước xử lý và sơ đồ mô-đun

```text
main (Leader)
 ├─ PatientStore (C): sở hữu toàn bộ bệnh nhân
 ├─ FileIO / DataGenerator (C): đọc, ghi, sinh đầu vào
 ├─ ConsoleUI (C): nhận lệnh, hiển thị, gọi chức năng
 └─ Simulation (Leader): dùng kho, cấu hình và đồng hồ
     ├─ Patient / SimulationConfig (Leader): kiểm tra, phân loại, reset
     ├─ selectRoom: chọn phòng đúng khoa theo (số chờ, mã phòng)
     ├─ ClinicRoom (A)
     │   ├─ LinkedQueue (A): FIFO thường
     │   └─ PriorityHeap (B): hàng ưu tiên
     └─ Statistics / Merge Sort (B): thống kê và báo cáo
```

Mã giả vòng mô phỏng:

```text
initialize(cấu hình):
    Kiểm tra cấu hình và tất cả đầu vào; lỗi thì trả false cùng thông báo
    Dọn tham chiếu cũ, reset bệnh nhân, tạo/cấu hình phòng và reset thống kê
    nextMinute = 0; initialized = true

step():
    Nếu chưa initialize hoặc isFinished(): trả false
    t = nextMinute
    Với từng phòng:
        Nếu finishIfDue(t) trả bệnh nhân: recordCompletion()
    Với từng người chưa đến có arrivalTime == t, ticket tăng dần:
        i = selectRoom(khoa của người đó)
        Ghi assignedRoomId; enqueue vào phòng i; đặt Waiting
    observeAllQueues()  // quan sát trước gọi khám
    Với từng phòng:
        p = startNext(t, usePriority)
        Nếu p khác nullptr: recordStart(*p)
    observeAllQueues()  // snapshot sau gọi khám
    Cập nhật đồng hồ; trả true

main:
    Tạo kho, đọc dữ liệu và xử lý lỗi
    Tạo mô phỏng dùng kho
    Lặp menu: nhập / chạy từng phút / chạy hết / xem / lưu / thoát
    Khi thoát: lưu đầu vào, dọn tài nguyên theo thứ tự sở hữu
```

Trong một bước, nhận người đến và xếp hàng là hai ý nghiệp vụ nhưng được xử lý nối tiếp cho từng người; mọi lượt xếp hàng phải xong trước giai đoạn gọi khám. `startNext` chịu trách nhiệm cập nhật `InService`, `startTime`, `finishTime`; Simulation không cộng thống kê khi hàm trả `nullptr`.

## 3. Ví dụ và kiểm tra dự kiến

### 3.1. Ví dụ bắt buộc: một phòng, ba người

Xét một phòng trong một khoa, bật ưu tiên, ticket lần lượt 1/2/3. Các phòng của khoa khác không có bệnh nhân nên không ảnh hưởng ví dụ.

| Người | Mức | Đến | Thời lượng |
|---|---:|---:|---:|
| BN01 | 2 — thường | 0 | 5 |
| BN02 | 2 — thường | 1 | 5 |
| BN03 | 0 — cấp cứu | 2 | 5 |

| Phút | Xử lý và trạng thái sau gọi khám |
|---|---|
| 0 | BN01 đến, vào FIFO rồi bắt đầu khám; dự kiến xong phút 5 |
| 1 | BN02 đến, chờ trong FIFO; BN01 tiếp tục khám |
| 2 | BN03 đến, chờ trong heap; BN01 không bị ngắt |
| 3–4 | BN01 tiếp tục; BN02 và BN03 chờ |
| 5 | Hoàn tất BN01, gọi BN03 từ heap, dự kiến xong phút 10 |
| 6–9 | BN03 khám; BN02 chờ |
| 10 | Hoàn tất BN03, gọi BN02 từ FIFO, dự kiến xong phút 15 |
| 11–14 | BN02 tiếp tục khám |
| 15 | Hoàn tất BN02; phòng rỗng |

| Thứ tự khám | Người | Bắt đầu | Kết thúc | Chờ = bắt đầu − đến |
|---:|---|---:|---:|---:|
| 1 | BN01 | 0 | 5 | 0 |
| 2 | BN03 | 5 | 10 | 3 |
| 3 | BN02 | 10 | 15 | 9 |

Mức 0 có trung bình/lớn nhất 3 phút; mức 2 có trung bình 4,5 phút, lớn nhất 9 phút; mức 1 chưa có dữ liệu. Số chờ lớn nhất là 2, không tính BN01 đang khám. Báo cáo chờ giảm dần: BN02 (9) → BN03 (3) → BN01 (0).

Phút 15 đã khám hết ba người nhưng với `closingTime = 480`, mô phỏng vẫn chưa kết thúc vì còn mở tiếp nhận. Sau xử lý phút 479, `nextMinute = 480`, nếu không có ai khác thì đủ điều kiện kết thúc. Nếu đặt riêng giờ đóng là 3 cho ví dụ này, mô phỏng kết thúc sau khi xử lý hoàn tất BN02 ở phút 15.

Đối chiếu khi tắt ưu tiên trên cùng đầu vào: BN01 khám 0–5, BN02 khám 5–10, BN03 khám 10–15; chờ lần lượt 0, 4, 8 phút. Đây là kết quả tính tay để kiểm tra khi cài đặt.

### 3.2. Các ca cần kiểm tra sau khi code

| Tình huống | Kết quả mong đợi |
|---|---|
| Kho rỗng, trước đóng cửa | Vẫn bước được, chưa kết thúc |
| Phòng rỗng, một người đến | Bắt đầu ngay trong phút đến, chờ 0 |
| Người mới đến đúng phút ca cũ xong | Hoàn tất ca cũ trước, xếp hàng rồi chọn người tiếp theo theo quy tắc |
| Cấp cứu đến khi phòng bận | Vào heap, không ngắt ca đang khám |
| Nhiều người cùng mức và phút đến | Ticket tăng dần quyết định thứ tự |
| Nhiều phòng cùng khoa hòa số chờ | Chọn mã phòng nhỏ nhất, cập nhật hàng trước khi phân người kế tiếp |
| Có phòng khoa khác ít người hơn | Không phân sai khoa |
| Đến phút 479 / 480 với cấu hình mặc định | 479 hợp lệ; 480 bị từ chối |
| Đóng cửa khi còn người chờ/đang khám | Khám tiếp tới hoàn tất, không nhận mới |
| Nhập tuổi âm, khoa ngoài 1–3, thời lượng 0 | Báo lỗi, không thêm vào kho |
| Mã hoặc ticket trùng | Báo lỗi, kho không thay đổi |
| Initialize với cấu hình/đầu vào sai | Trả false và lỗi trước khi reset trạng thái cũ |
| Chạy lại bật/tắt ưu tiên, đổi số phòng | Đầu vào giữ nguyên; trạng thái, đồng hồ và thống kê không cộng dồn |
| Hủy/reset sau khi đã xếp hàng | Không xóa Patient từ hàng/phòng, không để con trỏ tới Patient đã bị xóa |
| Kết thúc hợp lệ | Số bắt đầu = số hoàn tất = số bệnh nhân; không còn người trong hàng/phòng |

Các ca trên chưa được chạy; giai đoạn 1 chỉ xác định đầu vào và kết quả mong đợi.

## 4. Thời gian, bộ nhớ và so sánh

Gọi `N` là tổng bệnh nhân, `R` là tổng số phòng, `T` là số phút thực tế mô phỏng (gồm khám sau giờ đóng), `H` là số người trong heap của phòng đang xét. Chi phí chuỗi còn phụ thuộc độ dài mã/tên; bảng xem độ dài đó có giới hạn.

| Thao tác | Thời gian | Bộ nhớ phụ/được lưu |
|---|---|---|
| Phân loại ưu tiên, reset một Patient | O(1) | O(1) |
| Kiểm tra các trường số của một đầu vào | O(1) | O(1) |
| Reset trạng thái toàn kho | O(N) | O(1) phụ |
| Chọn phòng cho một người, duyệt mảng phòng | O(R) | O(1) phụ |
| Gọi hoàn tất/bắt đầu trên mọi phòng | O(R) cộng chi phí lấy hàng | O(1) phụ ngoài dữ liệu phòng |
| FIFO thêm/lấy | O(1) | O(1) node cho mỗi người đang chờ |
| Heap thêm/lấy | O(log(H+1)) thông thường; thêm phải tăng mảng có thể O(H) | Mảng theo sức chứa, giữ con trỏ |
| Quan sát tổng hàng chờ | O(R): tính tổng một lần rồi cập nhật từng phòng | O(1) phụ |
| Duyệt toàn kho tìm người đến mỗi phút, phương án đơn giản | O(N) mỗi phút | O(1) phụ |
| Kiểm tra trùng bằng quét kho cho từng bản ghi | O(N²) cho toàn bộ N bản ghi, nếu không có chỉ mục | O(1) phụ |
| Báo cáo bằng Merge Sort của B | O(N + m log(m+1)), m người đã bắt đầu | O(m) |

Thiết kế cơ sở dùng lượt duyệt kho mỗi phút cho dễ kiểm tra, phù hợp header hiện tại. Phần vòng mô phỏng có chặn trên O(T(N+R) + NR + N log(N+1)) khi xét chi phí heap khấu hao, chưa gồm đọc/kiểm tra trùng, xuất màn hình và tạo báo cáo. Không gọi cả chương trình là O(log N) chỉ vì dùng heap.

Kho đã có thứ tự `(arrivalTime, ticketNumber)` nên có thể tối ưu bằng con trỏ tới người chưa xử lý thay vì quét lại mỗi phút. Phương án này cần thống nhất cách cập nhật khi tiếp nhận thủ công và trạng thái thành viên trong `Simulation`; chưa tự thêm vào header ở giai đoạn này.

Bộ nhớ chính O(N + R) cho kho, node hàng, mảng con trỏ và phòng khi sức chứa được quản lý theo số bệnh nhân của kịch bản; báo cáo thêm O(N). Kho không bị sao chép thành một bản Patient cho mỗi hàng. So với mỗi phòng giữ bản sao bệnh nhân, dùng con trỏ tránh dữ liệu trạng thái khác nhau giữa các mô-đun nhưng đòi hỏi đúng thứ tự giải phóng.

## 5. Nguồn tham khảo và rà thiết kế tích hợp

Nguồn đặc tả nội bộ:

- [README — quy tắc chung](../../README.md).
- [Phân công giai đoạn 1 — phần Leader](../03_Phan_cong_giai_doan_1.md).
- [Tổng quan giai đoạn 1](../02_Tong_quan_giai_doan_1.md).
- [Patient.h](../../include/Patient.h), [SimulationConfig.h](../../include/SimulationConfig.h), [Simulation.h](../../include/Simulation.h).
- Đối chiếu [ClinicRoom.h](../../include/ClinicRoom.h), [PatientStore.h](../../include/PatientStore.h), [Statistics.h](../../include/Statistics.h), [PriorityHeap.h](../../include/PriorityHeap.h), [LinkedQueue.h](../../include/LinkedQueue.h).

Kết quả rà hợp đồng header:

| Phần | Điểm cần thống nhất khi tích hợp |
|---|---|
| Leader ↔ A | Simulation chọn phòng và ghi phòng/trạng thái chờ; A quản lý hàng trong phòng, cập nhật bắt đầu/kết thúc qua `startNext`/`finishIfDue` |
| A ↔ B | B trả con trỏ từ heap; A không xóa Patient; không ngắt ca đang khám; `waitingCount` cộng FIFO và heap |
| Leader ↔ B | Ghi bắt đầu/hoàn tất đúng một lần; reset trước kịch bản; quan sát hàng trước và sau gọi khám; thống kê theo ưu tiên gốc |
| Leader ↔ C | Kho sở hữu Patient và giữ địa chỉ ổn định; nhập thủ công qua `admit`; FileIO lưu đầu vào, không lưu con trỏ/trạng thái làm đầu vào mới |
| B ↔ C | Báo cáo duyệt `head()` chỉ đọc, không sửa thứ tự kho; bên gọi giải phóng mảng trước khi kho bị xóa/reset |

**Trạng thái duyệt:** đã đối chiếu các header liên quan, chưa thay đổi giao diện. Chưa có bản thiết kế A/B/C được bàn giao trong thư mục để duyệt chéo đầy đủ, vì vậy chưa xác nhận cả nhóm sẵn sàng sang giai đoạn 2.

Các điểm cần chốt trước khi code:

1. A/B/C xác nhận trách nhiệm cập nhật trạng thái và thời điểm quan sát hàng chờ theo tài liệu này; `enqueue` chưa ghi rõ trách nhiệm cập nhật trạng thái trong comment header, nên Leader đang nhận việc ghi `Waiting` và `assignedRoomId` ở Simulation.
2. Chọn duyệt kho mỗi phút cho bản cơ sở; nếu tối ưu bằng con trỏ tiến thì Leader cập nhật và thông báo header trước khi triển khai.
3. Thống nhất xử lý nhập sai/tệp sai và lỗi cấp phát giữa UI, FileIO và mô phỏng; không chạy tiếp với dữ liệu hỏng.
4. Các khung `.cpp` có tham chiếu tới `docs/03_Dac_ta_va_hop_dong_chung.md`, hiện chưa có tệp đó. Cần sửa tham chiếu khi bắt đầu cài đặt theo các tài liệu đặc tả đang tồn tại.

Không có câu hỏi thuật toán còn bỏ trống trong phạm vi Leader; các điểm trên là việc thống nhất tích hợp. Sau khi nhận thiết kế A/B/C, Leader rà ví dụ, quyền sở hữu và hợp đồng hàm rồi mới duyệt sang giai đoạn 2.
