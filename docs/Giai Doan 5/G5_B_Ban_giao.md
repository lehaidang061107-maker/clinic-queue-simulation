# Ban giao Giai doan 5 - B

**Phu trach:** `tests/bench.cpp`, target `bench` trong `Makefile`, so do hieu nang va nhan xet.

## 1. Da lam gi

Theo muc B cua [phan cong giai doan 5](../12_Phan_cong_giai_doan_5.md):

- Kiem tra va chay `make bench` voi bon cau hinh, nam kich thuoc va 5 lan lap.
- Luu ket qua tren Windows vao `results/bench_windows.csv`.
- Ve ba bieu do trong `docs/Giai Doan 5/img/`.
- Chay test rieng cua B va cac ca console lien quan.

## 2. Cach chay

- May: Windows 11 Home Single Language, 64-bit
- CPU: AMD Ryzen 7 7435HS, 16 luong logic
- Compiler: GCC 15.2.0 (MSYS2 MinGW64)
- Make: mingw32-make 3.82.90
- Co bien dich: `-O2`
- Commit: `2b0b039`
- Ngay do: 27/11/2026
- Du lieu: `generatePatients` seed 2026, gio dong 480, thoi luong 5-15

```bash
mingw32-make bench-quick
mingw32-make bench MACHINE=windows
mingw32-make test-b
mingw32-make test-console
```

## 3. Ket qua benchmark

CSV du 100 dong du lieu (`5 kich thuoc x 4 cau hinh x 5 lan`) va co 10 dong dau file gom thong tin may, he dieu hanh, compiler, co, commit va ngay do.

Bang trung vi thoi gian mo phong va Merge Sort:

| N | 1 phong, co uu tien | 1 phong, khong uu tien | 2 phong, co uu tien | 2 phong, khong uu tien | Merge Sort |
|---:|---:|---:|---:|---:|---:|
| 1.000 | 0,771 ms | 0,666 ms | 0,672 ms | 0,671 ms | 0,1205 ms |
| 2.000 | 1,548 ms | 1,379 ms | 1,381 ms | 1,397 ms | 0,2615 ms |
| 5.000 | 3,755 ms | 3,405 ms | 3,869 ms | 3,451 ms | 0,7035 ms |
| 10.000 | 7,995 ms | 7,611 ms | 7,436 ms | 7,661 ms | 1,5505 ms |
| 20.000 | 17,720 ms | 16,450 ms | 17,474 ms | 15,819 ms | 3,6130 ms |

Thoi gian mo phong tang gan 2,1–2,2 lan khi N tang tu 1.000 len 2.000 va tiep tuc tiep can 2x o cac muc lon. O 20.000 nguoi, 1 phong co/khong uu tien lan luot mat 17,720/16,450 ms; 2 phong co/khong uu tien lan luot mat 17,474/15,819 ms.

Merge Sort tang tu 0,1205 ms o N = 1.000 len 3,6130 ms o N = 20.000. Duong nay gan voi N log N theo truc log-log; ty le tang khoang 1,9–2,7 lan khi mot so kich thuoc lien tiep tang. Day phu hop voi do phuc tap sap xep cua bao cao.

## 4. Bieu do

- [Thoi gian mo phong theo N](img/b_thoi_gian_mo_phong.png)
- [Merge Sort va duong tham chieu N log N](img/b_merge_sort_nlogn.png)
- [Cho trung binh theo muc va kich ban](img/b_cho_trung_binh.png)

## 5. Kiem tra

| Kiem tra | Mong doi | Thuc te | Dat |
|---|---|---|---|
| `mingw32-make bench-quick` | Co thuc thi va ra CSV thu nhanh | 1000/2000 nguoi, 2 lan, deu chay xong | Dat |
| `mingw32-make bench MACHINE=windows` | Du 5 kich thuoc x 4 cau hinh x 5 lan, -O2 | 100 dong du lieu, Windows/GCC 15.2.0 | Dat |
| `mingw32-make test-b` | Cac test cua B PASS | 4 nhom PASS | Dat |
| `mingw32-make test-console` | 25 ca console dat | 25 dat, 0 chua dat | Dat |
| `mingw32-make test-all` | Toan bo unit test dat | A/B dat; C dung o assertion sinh lai `patients_day.txt` | Chua dat; loi khong thuoc phan B |

`test-all` moi dung khi `test-c` can so sanh tep sinh lai voi `data/patients_day.txt`. Do nay can C xac minh du lieu o workspace nay, khong sua vao phan cua B.

## 6. Con lai

- Them so do macOS sau toi uu neu Leader hoac nguoi co macOS chay `mingw32-make` khong the thay the.
- Neu may thu hai chay lai benchmark, can ghi cung format `results/bench_<may>.csv`.
