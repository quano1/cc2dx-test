all: configure prog.out
	@echo target: $@

ifeq ($(V), 1)
CMAKE := cmake
RM := rm
else
CMAKE := @cmake
RM := @rm
endif

configure: | .build
	@echo target: $@

.build: | cocos-project
	@echo target: $@
	$(CMAKE) -S. -GNinja -B.build || rm .build/CMakeCache.txt


# git apply --directory thirdparty/chipmunk ./patch/Chipmunk-7.0.1_cocos.patch

# git clone https://github.com/cocos2d/cocos2d-x -b v4 cocos
# wget https://github.com/cocos2d/cocos2d-x/archive/refs/tags/cocos2d-x-4.0.zip
# python2 download-deps.py
# git submodule update --init
cocos-project:
	@echo target: $@
	cocos new cocos-project -p com.tll.cocos.project -l cpp

prog.out:
	@echo target: $@
	$(CMAKE) --build .build $(if $(V),--verbose,)

clean:
	$(RM) -rf .build cocos-project

distclean: clean
	$(RM) -rf thirdparty

.PHONY: all clean configure
