---
layout: post
title:  "好用圣诞节项目"
date:   2019-12-14 18:00:00 +0000
categories: [HW, SW]
excerpt: "***"
---
本帖子琐谈是特别好用圣诞节项目，至少在nerd看来特别好用项目。基本上项目的能力很简单：它是一个夜间小灯，很多朋友夜间上厕所不想开灯（因为太亮了），小灯发的光不仅仅不太亮了，而且光颜色应该适合夜间。

# 3D设计
<style>
#box {
    border: 1px solid red;
    width: 50vw;
    height: 50vw;
}
</style>

<div id="box"></div>

<script src='{{ site.js | relative_url }}/src/three/three.min.js?v={{ site.version }}'></script>
<script src='{{ site.js | relative_url }}/src/three/STLLoader.js?v={{ site.version }}'></script>

<script type="text/javascript">
var container;
var camera, cameraTarget, scene, renderer;

function init() {
    container = document.getElementById('box');
    camera = new THREE.PerspectiveCamera(0, container.clientWidth / container.clientHeight, 1, 15);
    camera.position.set(3, 0.15, 3);
    cameraTarget = new THREE.Vector3(0, -0.25, 0);
    scene = new THREE.Scene();
    // Ground
    var plane = new THREE.Mesh(
        new THREE.PlaneBufferGeometry(40, 40),
        new THREE.MeshPhongMaterial({
            color: 0x999999,
            specular: 0x101010
        })
    );
    plane.rotation.x = -Math.PI / 2;
    plane.position.y = -0.5;
    scene.add(plane);
    plane.receiveShadow = true;
    // ASCII file
    var loader = new THREE.STLLoader();
    // Binary files
    var material = new THREE.MeshPhongMaterial({
        color: 0xAAAAAA,
        specular: 0x111111,
        shininess: 200
    });
    // Colored binary STL
    loader.load('{{ site.baseurl }}/assets/2019-12-15-xmas-project/box.stl', function(geometry) {
        var meshMaterial = material;
        if (geometry.hasColors) {
            meshMaterial = new THREE.MeshPhongMaterial({
                opacity: geometry.alpha,
                vertexColors: THREE.VertexColors
            });
        }
        var mesh = new THREE.Mesh(geometry, meshMaterial);
        mesh.position.set(0.5, 0.2, 0);
        mesh.rotation.set(-Math.PI / 2, Math.PI / 2, 0);
        mesh.scale.set(0.3, 0.3, 0.3);
        mesh.castShadow = true;
        mesh.receiveShadow = true;
        scene.add(mesh);
    });
    // Lights
    scene.add(new THREE.HemisphereLight(0x443333, 0x111122));
    addShadowedLight(1, 1, 1, 0xffffff, 1.35);
    addShadowedLight(0.5, 1, -1, 0xffaa00, 1);
    // renderer
    renderer = new THREE.WebGLRenderer({
        antialias: true
    });
    renderer.setPixelRatio(window.devicePixelRatio);
    renderer.setSize(container.clientWidth, container.clientHeight);
    renderer.shadowMap.enabled = true;
    container.appendChild(renderer.domElement);
    //
    window.addEventListener('resize', onWindowResize, false);
}

function addShadowedLight(x, y, z, color, intensity) {
    var directionalLight = new THREE.DirectionalLight(color, intensity);
    directionalLight.position.set(x, y, z);
    scene.add(directionalLight);
    directionalLight.castShadow = true;
    var d = 1;
    directionalLight.shadow.camera.left = -d;
    directionalLight.shadow.camera.right = d;
    directionalLight.shadow.camera.top = d;
    directionalLight.shadow.camera.bottom = -d;
    directionalLight.shadow.camera.near = 1;
    directionalLight.shadow.camera.far = 4;
    directionalLight.shadow.mapSize.width = 1024;
    directionalLight.shadow.mapSize.height = 1024;
    directionalLight.shadow.bias = -0.005;
}

function onWindowResize() {
    camera.aspect = container.clientWidth / container.clientHeight;
    camera.updateProjectionMatrix();
    renderer.setSize(container.clientWidth, container.clientHeight);
}

function animate() {
    requestAnimationFrame(animate);
    render();
}

function render() {
    var timer = Date.now() * 0.0005;
    camera.position.x = Math.cos(timer) * 3;
    camera.position.z = Math.sin(timer) * 3;
    camera.lookAt(cameraTarget);
    renderer.render(scene, camera);
}

init();
animate();
</script>

# 硬件设计

# 固件设计

# Demo