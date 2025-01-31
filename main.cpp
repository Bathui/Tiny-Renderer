// #include "tgaimage.h"
// #include "model.h"
// const TGAColor white = TGAColor(255, 255, 255, 255);
// const TGAColor red   = TGAColor(255, 0,   0,   255);
// const int width = 2000;
// const int height = 2000;

// void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
//     bool steep = false;
// 	if (std::abs(x0-x1) < std::abs(y0-y1)) {
// 		std::swap(x0, y0);
// 		std::swap(x1, y1);
// 		steep = true;
// 	}

// 	if (x1 < x0) {
// 		std::swap(x0, x1);
// 		std::swap(y0, y1);
// 	}
// 	int dx = x1 - x0;
// 	int dy = y1 - y0;
// 	int derror2 = std::abs(dy)*2;
// 	int error = 0;
// 	int y = y0;
// 	for (int x = x0; x <= x1; x++) { 
// 		if (steep){
// 			image.set(y, x, color);
// 		}
// 		else{
// 			image.set(x, y, color);
// 		}
// 		error += derror2;
// 		if (error> dx){
// 			y += (y1>y0? 1:-1);
// 			error -= 2*dx;
// 		}		
//     } 
// }
// // int main(int argc, char** argv) {
// // 	const char* name = nullptr;
// // 	if (argc == 1){
// // 		name = "output.tga";
// // 	}
// // 	else{
// // 		name = argv[1];
// // 	}
// // 	TGAImage image(width, height, TGAImage::RGB);
// // 	Model* model = new Model("african_head.obj");
// // 	// Model* model = new Model("sample.obj");
// // 	// std::cout<<model->num_faces()<<std::endl;
// //     std::cout<<model->num_verts()<<std::endl;

// // 	for (int i = 0; i < model->num_faces(); i++) { 
// // 		std::vector<int> face = model->face(i);
// // 		// std::cout<<(model->face(0))[6]<<std::endl;
// // 		for (int j = 0; j < 3; j++) {
// // 			vec3f v0 = model->vert(face[j*3]);
// // 			vec3f v1 = model->vert(face[(j+1)%3*3]);
// // 			int x0 = (v0.X() + 1.) * width/2.;
// // 			int x1 = (v1.X() + 1.) * width/2.;
// // 			int y0 = (v0.Y() + 1.) * height/2.;
// // 			int y1 = (v1.Y() + 1.) * height/2.;
// // 			line(x0, y0, x1, y1, image, white);
// // 		}
// // 	}

// // 	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
// // 	image.write_tga_file(name);
// // 	delete model;
// // }

