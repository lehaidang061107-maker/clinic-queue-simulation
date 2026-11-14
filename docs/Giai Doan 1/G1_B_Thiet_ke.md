# Giai đoạn 1 — B: Ưu tiên và thống kê

**Phạm vi:** `PriorityHeap`, `Statistics`, `makeSortedWaitReport` và Merge Sort. Thiết kế theo header hiện có; ví dụ là tính tay, chưa có kết quả thực thi.

## 1. Cách làm

### 1.1. Heap ưu tiên

Min-heap trên mảng động `Patient** items_`, với `size_` và `capacity_`. Heap chỉ mượn con trỏ; hủy mảng không xóa bệnh nhân. Constructor là `PriorityHeap()`; không thêm tham số vào giao diện. Chọn sức chứa khởi đầu dương, ví dụ 16, khi cài đặt.

`comesBefore(lhs, rhs)` so sánh lần lượt `(priority, arrivalTime, ticketNumber)` tăng dần: 0 cấp cứu trước 1 ưu tiên tuổi, sau đó đến sớm và ticket nhỏ trước. Mã và ticket duy nhất do C kiểm tra. A chỉ đưa mức 0/1 vào heap khi bật ưu tiên, mức 2 nằm FIFO; tắt ưu tiên thì mọi người nằm FIFO.

Mảng đánh chỉ số từ 0: cha `(i-1)/2` khi i > 0, con trái `2*i+1`, con phải `2*i+2`. Mỗi cha đứng trước hoặc bằng các con theo khóa so sánh, nên gốc là người cần lấy trước. Heap không bảo đảm toàn bộ mảng được sắp thứ tự.

### 1.2. Thống kê theo đúng header

`PriorityStats` chứa `startedCount`, `totalWait` kiểu `long long`, `maxWait`. `RoomStats` chứa `roomId`, `maxWaiting`. `Statistics` giữ `priority_[3]`, mảng động `rooms_`, `roomCount_`, `maxSystemWaiting_`, `completedCount_`.

- `reset(roomCount)` dọn mảng cũ, tạo thống kê phòng mới với roomId bằng index + 1, đưa các bộ đếm/cực đại về 0. Destructor giải phóng mảng; không sao chép đối tượng sở hữu mảng.
- `recordStart(const Patient&)` được Simulation gọi đúng một lần khi `startNext` trả người bắt đầu khám. Tính chờ bằng `startTime - arrivalTime`, tăng startedCount, cộng tổng và cập nhật max theo **mức ưu tiên gốc**, kể cả kịch bản tắt ưu tiên.
- `recordCompletion()` chỉ tăng số hoàn tất khi `finishIfDue` trả người vừa xong. Không cộng thời gian chờ lần nữa.
- `observeQueues(roomIndex, roomWaiting, systemWaiting)` cập nhật max phòng và max hệ thống. Simulation tính tổng hàng của tất cả phòng tại cùng thời điểm rồi truyền vào; không cộng các cực đại từng phòng.
- `byPriority`, `byRoom`, `maxSystemWaiting`, `completedCount` là các hàm đọc. Chỉ số phòng phải hợp lệ.

Trung bình = `double(totalWait) / startedCount`; khi startedCount bằng 0, UI in **“Chưa có dữ liệu”** cho trung bình/lớn nhất, không chia cho 0. Thống kê chờ tính cả người đang khám đã bắt đầu, không cần đợi Completed.

Quan sát hàng sau khi xếp hết người đến, trước gọi khám, và sau gọi khám theo Leader. Người đang khám không thuộc hàng chờ. Số đếm bắt đầu/hoàn tất chỉ thay đổi bởi các sự kiện tương ứng, không thay đổi khi UI xem lại báo cáo.

### 1.3. Báo cáo bằng Merge Sort

`const Patient** makeSortedWaitReport(const PatientStore&, int& count)` tạo mảng con trỏ chỉ đọc tới người có `startTime >= 0` (đang khám hoặc đã xong). Sắp thời gian chờ giảm dần, hòa thì ticket tăng dần. Không sửa thứ tự danh sách kho. Bên gọi `delete[]` mảng sau khi dùng, không xóa Patient; dùng xong trước khi reset hoặc xóa kho.

## 2. Các bước xử lý và hình

### 2.1. Heap

```text
push(p):                         // p khác nullptr
    Nếu size == capacity: grow()
    items[size] = p; tăng size; siftUp(size-1)

siftUp(i):
    Khi i > 0 và items[i] đứng trước cha:
        Đổi chỗ với cha; i = chỉ số cha

pop():
    Nếu rỗng: trả nullptr
    Lưu items[0]; giảm size
    Nếu size > 0: đưa phần tử cuối cũ lên gốc; siftDown(0)
    Trả con trỏ đã lưu

siftDown(i):
    Tìm người đứng trước nhất trong node i và các con còn nằm trong size
    Nếu i đã tốt nhất: dừng
    Đổi chỗ i với con tốt nhất; tiếp tục từ con đó

grow():
    Kiểm tra sức chứa mới không tràn và dương
    Cấp phát mảng gấp đôi, chép size con trỏ
    Chỉ sau khi thành công mới xóa mảng cũ và thay items/capacity
```

`peek()` trả `const Patient*` ở gốc hoặc nullptr. `isEmpty/size` đọc bộ đếm. `clear()` chỉ đặt size = 0, giữ sức chứa; destructor `delete[] items_`. Nếu cấp phát thất bại, giữ cấu trúc cũ hợp lệ và truyền lỗi cấp phát lên điều phối; không im lặng làm mất người.

### 2.2. Thống kê và báo cáo

```text
recordStart(p):
    k = mức ưu tiên gốc; w = p.startTime - p.input.arrivalTime
    priority[k].startedCount += 1
    priority[k].totalWait += w
    priority[k].maxWait = max hiện tại và w (so sánh bằng if)

makeSortedWaitReport(store, count):
    Duyệt head() đếm m người có startTime >= 0
    Nếu m == 0: count = 0; trả nullptr
    Cấp phát m con trỏ const Patient* và một mảng phụ cùng cỡ
    Duyệt kho lần hai điền mảng
    Merge Sort theo (chờ giảm, ticket tăng)
    Giải phóng mảng phụ; count = m; trả mảng kết quả

mergeSort(left, right):
    Nếu left >= right: dừng
    mid = left + (right-left)/2
    Sắp nửa trái, sắp nửa phải
    Trộn hai nửa vào mảng phụ rồi chép về
```

Khi trộn, chọn người chờ lâu hơn; nếu bằng chờ, chọn ticket nhỏ hơn; nếu khóa bằng hoàn toàn thì chọn nửa trái để giữ ổn định. Dùng `const Patient**` xuyên suốt báo cáo, không bỏ const. Nếu cấp phát báo cáo thất bại, dọn mảng đã cấp phát và truyền lỗi, không trả báo cáo thiếu như thành công.

## 3. Ví dụ và kiểm tra dự kiến

### 3.1. Heap bắt buộc

| Người | Mức | Phút đến | Ticket |
|---|---:|---:|---:|
| BN01 | 1 | 1 | 1 |
| BN02 | 0 | 2 | 2 |
| BN03 | 1 | 1 | 3 |

Thêm BN01: `[BN01]`. Thêm BN02: đặt cuối rồi đổi với cha → `[BN02, BN01]`. Thêm BN03: `[BN02, BN01, BN03]`.

```text
        BN02 (0,2,2)
        /          \
 BN01 (1,1,1)    BN03 (1,1,3)
```

Pop BN02: BN03 lên gốc rồi đổi với BN01 → `[BN01, BN03]`. Pop tiếp trả BN01 rồi BN03. BN01 thắng BN03 nhờ ticket nhỏ hơn khi cùng mức và phút đến.

### 3.2. Thống kê và Merge Sort

Hai người cùng mức có thời gian chờ 4 và 8: startedCount = 2, totalWait = 12, trung bình 6 phút, lớn nhất 8. Nếu mới một người hoàn tất thì completedCount = 1 nhưng thống kê chờ vẫn tính cả hai người đã bắt đầu. Mức chưa có ai bắt đầu: UI in “Chưa có dữ liệu”.

Ví dụ Leader: BN01 chờ 0, BN02 chờ 9, BN03 chờ 3 → báo cáo BN02 → BN03 → BN01. Nếu hai người cùng chờ 4, ticket 2 đứng trước ticket 5.

Để phân biệt cực đại hệ thống với tổng cực đại phòng: thời điểm A có số chờ (3,0), thời điểm B có (0,4). Max từng phòng là 3 và 4; max hệ thống là 4, không phải 7.

| Ca dự kiến | Kết quả mong đợi |
|---|---|
| Heap rỗng / một phần tử | peek/pop null khi rỗng; lấy người cuối không đọc con ngoài mảng |
| Cùng mức, khác giờ đến | Đến trước lấy trước |
| Cùng mức và giờ đến | Ticket nhỏ lấy trước |
| Mảng đầy | Tăng mảng, không mất con trỏ, thứ tự pop vẫn đúng |
| clear rồi thêm lại | size về 0, dùng lại được, không xóa Patient |
| Chưa có ai bắt đầu | Báo cáo nullptr/count 0; không chia cho 0 |
| Có người đang khám | Được đưa vào báo cáo và thống kê chờ |
| Người chưa đến/đang chờ | Không có trong báo cáo chờ đã xác định |
| Xem báo cáo nhiều lần | Không tăng bộ đếm hay đổi thứ tự kho |
| Reset đổi số phòng | Không giữ thống kê kịch bản cũ, không rò mảng phòng |
| Hàng trước/sau gọi khám | Ghi được đỉnh trước gọi khám, không tính người đang khám |

## 4. Thời gian, bộ nhớ và so sánh

Gọi H là số phần tử heap, C là sức chứa, N là số bệnh nhân trong kho, m là số người đã bắt đầu, R là số phòng.

| Thao tác | Thời gian | Bộ nhớ |
|---|---|---|
| Heap push thông thường / pop | O(log(H+1)) | O(1) phụ |
| grow và lần push cần grow | O(H) | O(C) mảng mới trong lúc sao chép |
| push qua nhiều lần tăng gấp đôi | O(log(H+1)) khấu hao, gồm vun heap | Heap O(C) |
| peek/size/isEmpty/clear | O(1) | Giữ mảng O(C) |
| recordStart/recordCompletion/observeQueues | O(1) mỗi lần | O(1) |
| Simulation quan sát tất cả phòng | O(R), tính tổng một lần | O(1) phụ |
| reset thống kê | O(R cũ + R mới) chặn trên | O(R mới) lưu trữ |
| Tạo và sắp báo cáo | O(N + m log(m+1)) | O(m), gồm kết quả và mảng phụ; ngăn xếp O(log(m+1)) |

Danh sách liên kết luôn sắp theo ưu tiên mất O(H) tìm chỗ thêm, nhưng lấy đầu O(1). Heap cân bằng thêm/lấy O(log H), lưu liên tục nhưng phải tăng mảng và không hỗ trợ duyệt toàn bộ theo thứ tự chỉ bằng đọc mảng. Một lần push có grow là O(H), không được ghi mọi lần thêm đều O(log H). Merge Sort bảo đảm O(m log m) và ổn định khi trộn đúng, đổi lại cần mảng phụ; không dùng `std::sort` hay container STL trong lõi.

## 5. Nguồn tham khảo và điểm tích hợp

- [Phân công](../03_Phan_cong_giai_doan_1.md), [Tổng quan](../02_Tong_quan_giai_doan_1.md).
- [PriorityHeap.h](../../include/PriorityHeap.h), [Statistics.h](../../include/Statistics.h), [PatientStore.h](../../include/PatientStore.h).
- [Leader](G1_Leader_Thiet_ke.md), [A](G1_A_Thiet_ke.md), [C](G1_C_Thiet_ke.md).

B dùng đúng recordStart/recordCompletion và hai thời điểm quan sát hàng của Leader. Báo cáo chỉ mượn dữ liệu từ kho của C; A chịu trách nhiệm chọn heap/FIFO. B đọc C để đối chiếu thứ tự kho và vòng đời báo cáo; A đọc B để đối chiếu heap. Không còn câu hỏi thuật toán bỏ trống trong phạm vi B; các ca trên cần chạy sau khi cài đặt.
