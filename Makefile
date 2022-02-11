all: configure prog.out
	@echo target: $@

# ifeq ($(V), 1)
CMAKE := cmake
RM := rm
# else
# CMAKE := @cmake
# RM := @rm
# endif

configure: | .build
	@echo target: $@

# git apply --directory thirdparty/chipmunk ./patch/Chipmunk-7.0.1_cocos.patch

# git clone https://github.com/cocos2d/cocos2d-x -b v4 cocos
# wget https://github.com/cocos2d/cocos2d-x/archive/refs/tags/cocos2d-x-4.0.zip
# python2 download-deps.py
# git submodule update --init

#libigl
#chipmunk

thirdparty-clone:
	if [ ! -d "thirdparty/libigl" ]; then \
		git clone https://github.com/libigl/libigl thirdparty/libigl -b v2.3.0; \
	fi;
	if [ ! -d "thirdparty/chipmunk" ]; then \
		git clone https://github.com/slembcke/Chipmunk2D thirdparty/chipmunk -b Chipmunk-7.0.1 && \
		git apply --directory thirdparty/chipmunk ./patch/Chipmunk-7.0.1_cocos.patch; \
	fi;
	if [ ! -d "thirdparty/clipper" ]; then \
		git clone https://github.com/Geri-Borbas/Clipper thirdparty/clipper; \
	fi;

thirdparty-install: thirdparty-clone
	cmake -S thirdparty/chipmunk -GNinja -B.thirdparty/chipmunk -DBUILD_DEMOS=OFF -DINSTALL_DEMOS=OFF -DBUILD_SHARED=OFF -DBUILD_STATIC=ON -DINSTALL_STATIC=ON -DCMAKE_INSTALL_PREFIX=.install
	cmake --build .thirdparty/chipmunk
	cmake --install .thirdparty/chipmunk
	cmake -S thirdparty/clipper/cpp -GNinja -B.thirdparty/clipper -DCMAKE_INSTALL_PREFIX=.install
	cmake --build .thirdparty/clipper
	cmake --install .thirdparty/clipper


# -DLIBIGL_WITH_TRIANGLE=ON -DLIBIGL_USE_STATIC_LIBRARY=OFF -DLIBIGL_WITH_CGAL=OFF -DLIBIGL_WITH_COMISO=OFF -DLIBIGL_WITH_CORK=OFF
# -DLIBIGL_WITH_EMBREE=OFF -DLIBIGL_WITH_MATLAB=OFF -DLIBIGL_WITH_MOSEK=OFF -DLIBIGL_WITH_OPENGL=OFF -DLIBIGL_WITH_OPENGL_GLFW=OFF
# -DLIBIGL_WITH_OPENGL_GLFW_IMGUI=OFF -DLIBIGL_WITH_PNG=OFF -DLIBIGL_WITH_TETGEN=OFF -DLIBIGL_WITH_PREDICATES=OFF -DLIBIGL_WITH_XML=OFF
# -DLIBIGL_WITHOUT_COPYLEFT=OFF -DLIBIGL_EXPORT_TARGETS=OFF -DLIBIGL_BUILD_TESTS=OFF -DLIBIGL_BUILD_TUTORIALS=OFF


# -DBUILD_DEMOS=OFF
# -DINSTALL_DEMOS=OFF
# -DBUILD_SHARED=OFF
# -DBUILD_STATIC=ON
# -DINSTALL_STATIC=ON

# -DBUILD_SHARED_LIBS=OFF

clean:
	$(RM) -rf .build

thirdparty-clean:
	$(RM) -rf .thirdparty .install

distclean: clean thirdparty-clean
	$(RM) -rf thirdparty cocos-project

.PHONY: all clean distclean configure thirdparty-clone thirdparty-install thirdparty-clean


.build: thirdparty-install | cocos-project
	@echo target: $@
	$(CMAKE) -S. -GNinja -B.build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_INSTALL_PREFIX=.install

cocos-project:
	@echo target: $@
	cocos new cocos-project -p com.tll.cocos.project -l cpp

prog.out:
	@echo target: $@
	$(CMAKE) --build .build $(if $(V),--verbose,)

