# Perfect Hashing

## Mô tả
Dự án này triển khai **Perfect Hashing** sử dụng kĩ thuật **băm hai cấp** để đảm bảo **không xảy ra va chạm** (Collision). Mục tiêu là để tối ưu hóa tốc độ tìm kiếm dữ liệu trong tập dữ liệu lớn với thời gian là hằng số **O(1)**.

## Tính năng
- Đọc dữ liệu từ file văn bản.
- Sử dụng hàm băm phổ quát (universal hashing).
- Băm hai cấp để tránh va chạm.
- Tìm kiếm từ khóa với thời gian là hằng số.
- Đo thời gian, bộ nhớ xây dựng bảng băm và thời gian tìm kiếm với các dạng dữ liệu đầu vào khác nhau (sequential, random, clustered).
- So sánh hiệu năng với Modulo Hashing.

## Cài đặt

### Yêu cầu:
- Visual Studio 2022 hoặc trình biên dịch C++ hỗ trợ C++17 trở lên.

### Cách chạy:
1. Mở file `main.cpp` trong Visual Studio 2022 hoặc trình biên dịch có hỗ trợ C++.
2. Đảm bảo file dữ liệu đầu vào (ví dụ: `Test1_Sequential.txt`) có định dạng mỗi đoạn (không xuống dòng) là một chuỗi, trong đó từ khóa là từ đầu tiên.
3. Biên dịch và chạy chương trình.
4. Nhập đường dẫn đến file dữ liệu khi được yêu cầu.
5. Nhập số lượng từ khóa và từ khóa cần tra cứu khi được yêu cầu.


## Cấu trúc file

- `main.cpp`: Chứa toàn bộ mã nguồn của chương trình.
- `Data.zip`: File nén chứa dữ liệu đầu vào (các file .txt).
- `README.md`: Tài liệu mô tả dự án.


## Ví dụ dữ liệu

```txt
Apple  n. 1 roundish firm fruit with crisp flesh. 2 tree bearing this.  apple of one's eye cherished person or thing. [old english]
Banana  n. 1 long curved soft fruit with a yellow skin. 2 treelike plant bearing it.  go bananas slang go mad. [portuguese or spanish, from an african name]
Bike  colloq. —n. Bicycle or motor cycle. —v. (-king) ride a bike.  biker n. [abbreviation]
Bronze medal  n. Medal, usu. Awarded as third prize.
Dirty  —adj. (-ier, -iest) 1 soiled, unclean. 2 causing dirtiness (dirty job).
```

## Ví dụ cách chạy
```txt
Input the path of file: Test1_Sequential.txt

PERFECT HASHING:
Run time: 957ms         Memory: 74 MB

MODULO HASHING:
Run time: 599ms         Memory: 35 MB

------------------------------------------------------

Input the numbers of keys which you want to SEARCH: 2
Input 2 keys:
Apple
Phone

Result of Perfect Hashing:

Apple  n. 1 roundish firm fruit with crisp flesh. 2 tree bearing this.  apple of one's eye cherished person or thing. [old english]

Phone book  n. = *telephone directory.

Run time: 235 microseconds

Result of Modulo Hashing:

Apple  n. 1 roundish firm fruit with crisp flesh. 2 tree bearing this.  apple of one's eye cherished person or thing. [old english]

Phone book  n. = *telephone directory.

Run time: 1320 microseconds
```

