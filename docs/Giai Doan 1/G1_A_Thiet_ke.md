# Giai đoạn 1 — A: Hàng đợi thường và phòng khám

**Phạm vi:** `LinkedQueue`, `ClinicRoom`. Đây là thiết kế và kết quả tính tay, chưa phải kết quả chạy chương trình. Giữ nguyên giao diện header.

## 1. Cách làm

### 1.1. Hàng đợi `LinkedQueue`

FIFO dùng danh sách liên kết đơn. Mỗi `Node` chứa `Patient* patient` và `Node* next`. `front_` trỏ đầu hàng, `rear_` trỏ cuối, `size_` đếm người chờ. Khi rỗng, cả hai con trỏ bằng `nullptr` và kích thước bằng 0.

Người mới nối vào cuối; người được gọi lấy từ đầu. Hàng chỉ sở hữu node, không sở hữu bệnh nhân. `PatientStore` giữ bệnh nhân nên `dequeue`, `clear` và hàm hủy chỉ xóa node. Không sao chép hàng vì copy constructor/toán tử gán đã bị xóa trong header.

### 1.2. Phòng `ClinicRoom`

Phòng giữ `roomId_`, `specialtyId_`, FIFO `normal_`, heap `priority_` và con trỏ mượn `current_` tới người đang khám. Không cần thêm biến giờ kết thúc: dùng `current_->finishTime`.

- `configure(roomId, specialtyId)` chỉ gọi khi phòng không có người chờ và không đang khám.
- `enqueue(patient, usePriority)` nhận con trỏ hợp lệ do Simulation phân đúng khoa. Khi bật ưu tiên, **chỉ mức 0/1 vào heap**, mức 2 vào FIFO. Khi tắt ưu tiên, mọi mức vào FIFO.
- Simulation chịu trách nhiệm ghi `Waiting` và `assignedRoomId` khi tiếp nhận vào phòng; `enqueue` chỉ chọn và cập nhật cấu trúc hàng.
- `startNext(minute, usePriority)` không gọi người mới khi đang bận. Bật ưu tiên thì lấy heap trước, heap rỗng mới lấy FIFO; tắt ưu tiên chỉ lấy FIFO. Không đổi chế độ giữa kịch bản; muốn đổi phải `initialize` lại, dọn hàng và reset bệnh nhân.
- `startNext` ghi `InService`, mã phòng, `startTime = minute`, `finishTime = minute + serviceDuration`. Simulation ghi thống kê nếu hàm trả người vừa bắt đầu.
- `finishIfDue(minute)` chuyển người hiện tại sang `Completed` khi đã tới giờ xong, bỏ `current_` và trả con trỏ cho Simulation ghi hoàn tất đúng một lần.

A chỉ quản lý một phòng. Chọn phòng ít người chờ nhất đúng khoa, hòa theo mã phòng, là việc của Leader. Cấp cứu đến không ngắt ca đang khám. `waitingCount()` bằng FIFO + heap, không gồm `current_`.

## 2. Các bước xử lý và hình

### 2.1. FIFO

```text
enqueue(p):                 // tiền điều kiện p khác nullptr
    Cấp phát node mới (p, nullptr) trước khi thay đổi hàng
    Nếu rỗng: front = rear = node mới
    Ngược lại: rear.next = node mới; rear = node mới
    Tăng size

dequeue():
    Nếu rỗng: trả nullptr
    Giữ node đầu và con trỏ bệnh nhân
    front = front.next; giảm size
    Nếu front == nullptr: rear = nullptr
    Xóa node cũ, trả con trỏ bệnh nhân

clear():
    Duyệt và xóa từng node, không xóa Patient
    front = rear = nullptr; size = 0
```

`LinkedQueue()` khởi tạo hàng rỗng; `~LinkedQueue()` gọi `clear()`. `peek()` trả `const Patient*` ở đầu hoặc `nullptr`, không lấy khỏi hàng. `isEmpty()` kiểm tra rỗng; `size()` trả bộ đếm.

### 2.2. Phòng khám

```text
enqueue(p, usePriority):
    Nếu usePriority và p.priority thuộc {0, 1}: priority.push(p)
    Ngược lại: normal.enqueue(p)

startNext(t, usePriority):
    Nếu current khác nullptr: trả nullptr
    Nếu usePriority và heap không rỗng: p = priority.pop()
    Ngược lại: p = normal.dequeue()
    Nếu p == nullptr: trả nullptr
    current = p
    Ghi InService, assignedRoomId, startTime, finishTime
    Trả p

finishIfDue(t):
    Nếu current == nullptr hoặc t < current.finishTime: trả nullptr
    p = current; p.status = Completed; current = nullptr
    Trả p

reset():
    normal.clear(); priority.clear(); current = nullptr
```

`ClinicRoom()` đặt mã phòng/khoa là -1, `current_ = nullptr`; hai hàng khởi tạo rỗng. `reset()` dọn tham chiếu, không tự reset trạng thái Patient: Simulation gọi `PatientStore::resetStates()` sau đó. Các getter `id`, `specialtyId`, `isBusy`, `normalQueueSize`, `priorityQueueSize`, `waitingCount`, `currentPatient` chỉ đọc dữ liệu; `currentPatient()` trả `const Patient*`.

Nếu cấp phát node/heap thất bại, không được bỏ mất người hay coi là thêm thành công; lỗi phải truyền lên để dừng thao tác/kịch bản và dọn tài nguyên. Các hàm thêm kiểu `void` không có mã lỗi nên có thể truyền ngoại lệ cấp phát; chi tiết bắt lỗi thuộc lớp điều phối khi cài đặt.

## 3. Ví dụ và kiểm tra dự kiến

### 3.1. Thêm, lấy và dùng lại FIFO

```text
Ban đầu: front -> BN01 -> BN02 -> BN03 -> nullptr
                                rear ^           size = 3
Thêm BN04:
         front -> BN01 -> BN02 -> BN03 -> BN04 -> nullptr
                                         rear ^  size = 4
Lấy một: trả BN01
         front -> BN02 -> BN03 -> BN04 -> nullptr
                                rear ^           size = 3
Lấy hết: trả BN02, BN03, BN04
         front = rear = nullptr; size = 0
Lấy tiếp: trả nullptr; size vẫn 0
Thêm BN05:
         front = rear -> BN05 -> nullptr; size = 1
```

### 3.2. Đối chiếu ví dụ Leader

Một phòng, mỗi ca 5 phút: BN01 thường đến 0, BN02 thường đến 1, BN03 cấp cứu đến 2. Bật ưu tiên: BN01 khám 0–5; BN03 khám 5–10; BN02 khám 10–15. Thời gian chờ tương ứng 0, 3, 9 phút. BN02 nằm FIFO, BN03 nằm heap; BN01 không bị ngắt. Tắt ưu tiên trên cùng đầu vào: BN01 → BN02 → BN03, chờ 0, 4, 8 phút.

| Ca dự kiến | Kết quả mong đợi |
|---|---|
| Hàng rỗng | `peek/dequeue` trả nullptr, `clear` an toàn |
| Một người rồi lấy ra | Cả front/rear về nullptr, size về 0; thêm lại được |
| Nhiều người | FIFO giữ thứ tự thêm, không xóa bệnh nhân |
| Phòng rảnh và hai hàng rỗng | `startNext` trả nullptr |
| Phòng bận có cấp cứu mới đến | Không đổi current hoặc giờ kết thúc |
| Bật ưu tiên có cả ba mức | Mức 0/1 ở heap, mức 2 ở FIFO; lấy heap trước |
| Tắt ưu tiên | Cả ba mức vào FIFO, lấy theo thứ tự đến/ticket do Simulation đưa vào |
| Trước/đúng giờ xong | Trước giờ trả nullptr; đúng giờ trả người hoàn tất một lần |
| Hai người chờ, một người khám | `waitingCount() == 2` |
| Reset khi còn hàng/người khám | Hàng rỗng, current null, Patient vẫn thuộc kho |

## 4. Thời gian, bộ nhớ và so sánh

Gọi Q là số người trong FIFO, H là số người trong heap, C là sức chứa heap.

| Thao tác | Thời gian | Bộ nhớ |
|---|---|---|
| FIFO thêm/lấy/peek/size/isEmpty | O(1) | Thêm một node O(1); tổng O(Q) |
| FIFO clear/hủy | O(Q) | O(1) phụ |
| Phòng enqueue thường | O(1) | Một node |
| Phòng enqueue ưu tiên | O(log(H+1)) thông thường; O(H) khi tăng mảng | Heap O(C) |
| startNext lấy FIFO / heap | O(1) / O(log(H+1)) | O(1) phụ |
| finishIfDue, getter | O(1) | O(1) |
| reset phòng | O(Q), vì heap clear chỉ đặt size về 0 | Có thể giữ mảng heap |

DSLK tăng theo số người, không cần dịch phần tử, nhưng tốn con trỏ và cấp phát từng node, truy cập bộ nhớ kém liên tục. Hàng trên mảng nếu dịch sau mỗi lần lấy tốn O(Q); mảng vòng cũng thêm/lấy O(1), nhưng phải quản lý chỉ số vòng và tăng mảng khi đầy. Nhóm dùng DSLK theo phương án đã chốt.

## 5. Nguồn tham khảo và điểm tích hợp

- [Phân công](../03_Phan_cong_giai_doan_1.md), [Tổng quan](../02_Tong_quan_giai_doan_1.md).
- [LinkedQueue.h](../../include/LinkedQueue.h), [ClinicRoom.h](../../include/ClinicRoom.h), [Patient.h](../../include/Patient.h).
- [Thiết kế Leader](G1_Leader_Thiet_ke.md), [Thiết kế B](G1_B_Thiet_ke.md): hợp đồng heap, trạng thái và thống kê.

Đã dùng quy ước của Leader: Simulation ghi Waiting/mã phòng; A ghi bắt đầu/hoàn tất; không đổi cấu hình giữa kịch bản. Ví dụ và kiểm tra ở trên là dự kiến, chưa thực thi. A đọc B để đối chiếu heap; C đọc A để đối chiếu quyền sở hữu và cách UI lấy số người chờ. Không còn câu hỏi thuật toán bỏ trống trong phạm vi A; Leader duyệt tích hợp trước khi code.
