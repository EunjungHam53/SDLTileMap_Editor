### Họ và tên: Lê Xuân Hùng
### MSV: 22028172

# Tool Edit TileMap

##### 1. Tổng quan về tool:
Tham khảo ý tưởng cách dựng tilemap bằng Unity, mình tạo ra một công cụ chỉnh sửa map và một cách trực tiếp (thêm và bỏ các ô tile) cũng như có [1 file data làm đầu ra](https://github.com/EunjungHam53/SDLTileMap_Editor/blob/main/map.dat) của chương trình. File data này sẽ được sử dụng bên game chính [SWEET OASIS](https://github.com/EunjungHam53/SweetOasis_LTNC) để thực hiện việc load 60 ảnh tiles lên màn hình và thực hiện các chức năng của tile map.

##### 2. Cách sử dụng tool:
### *Những phím đặc biệt*:
- SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT: dùng để điều hướng màn hình di chuyển lên xuống trái phải
- SDLK_s: lưu lại toàn bộ trạng thái của map ( cập nhật dữ liệu vào file data)
- SDL_BUTTON_LEFT: thêm 1 ô vào vị trí chuột
- SDL_BUTTON_RIGHT: xóa ô ở vị trí chuột

### *Những phím tùy chỉnh với file ảnh các tile đã được đánh số*:
- [File các ảnh tile](https://github.com/EunjungHam53/SDLTileMap_Editor/tree/main/map2)
- Từ dòng số 525 đến dòng 660 trong file main.cpp
- Trong 1 lượt điều chỉnh sửa map ta có thể thêm xóa lưu đến khoảng 35 kiểu ô tile 
