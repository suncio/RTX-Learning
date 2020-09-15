# RTX-Learning Devlog



##### 09/14/2020

- "Ray Tracing The Next Week" Ongoing

- Motion Blur

  - Add time parameter to ray
  - Moving sphere as Hittable

- BVH

- Solid Color Texture, Checker Texture & Image Texture

  ![checker](./img/img_checker_two_spheres.png)

  ![earth](./img/img_earth.png)



##### 09/10/2020

- "Ray Tracing in One Weekend" Complete

- Cross-Product bug caused shear:

  - Wrong: `return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - rhs.x);`

    ![wrong](./img/img_7_7_7.png)

  - Right: `return Vector3(y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x);`

    ![correct](./img/img_7_7_7_correct.png)

- Final Scene:

  ![Final Scene Image](./img/img_final_scene.png)





##### 09/07/2020

- random utility
	- C++ \<random\> header 
- `camera.h `
- Antialiasing
- Diffuse Material
- Gamma Correction 



##### 09/06/2020

- Basic code structure 

- Classes:

  - Math: Vector3, MathUtils
  - Ray
  - Hittable -> Sphere/HittableList

- Img:

  ![img01](./img/img01.png)