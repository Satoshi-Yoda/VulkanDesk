#v0.7

VULKAN_HOME     = C:/_soft/VulkanSDK_1.2.148.1
GLFW_HOME       = O:/_libs/glfw-3.3.2.bin.WIN64
GLM_HOME        = O:/_libs/glm
STB_HOME        = O:/_repo/stb
VMA_HOME        = O:/_libs/VulkanMemoryAllocator-master/src
MAGIC_ENUM_HOME = O:/_libs/magic_enum-master/include
CATCH_HOME      = O:/_libs/Catch2-amalgamated

VULKAN_INCLUDE     = ${VULKAN_HOME}/Include
GLFW_INCLUDE       = ${GLFW_HOME}/include
GLM_INCLUDE        = ${GLM_HOME}
STB_INCLUDE        = ${STB_HOME}
VMA_INCLUDE        = ${VMA_HOME}
MAGIC_ENUM_INCLUDE = ${MAGIC_ENUM_HOME}
CATCH_INCLUDE      = ${CATCH_HOME}

VULKAN_LIB = ${VULKAN_HOME}/Lib
GLFW_LIB   = ${GLFW_HOME}/lib-mingw-w64

$(info mode = ${mode})
ifeq (${mode}, release)
	INCLUDE = -I${VULKAN_INCLUDE} -I${GLFW_INCLUDE} -I${GLM_INCLUDE} -I${STB_INCLUDE} -I${VMA_INCLUDE} -I${MAGIC_ENUM_INCLUDE}
	LINK = -ljsoncpp ${VULKAN_LIB}/vulkan-1.lib ${GLFW_LIB}/glfw3.dll
	COMPILE = -std=c++20 -O3 -flto -mavx -Wno-deprecated-declarations
	DEFINE = -DNDEBUG
else ifeq (${mode}, debug)
	INCLUDE = -I${VULKAN_INCLUDE} -I${GLFW_INCLUDE} -I${GLM_INCLUDE} -I${STB_INCLUDE} -I${VMA_INCLUDE} -I${MAGIC_ENUM_INCLUDE}
	LINK = -ljsoncpp ${VULKAN_LIB}/vulkan-1.lib ${GLFW_LIB}/glfw3.dll
	COMPILE = -std=c++20 -O1 -Wno-deprecated-declarations -Wall -g
	DEFINE = -Duse_validation
else ifeq (${mode}, tests)
	INCLUDE = -I${VULKAN_INCLUDE} -I${GLFW_INCLUDE} -I${GLM_INCLUDE} -I${STB_INCLUDE} -I${VMA_INCLUDE} -I${MAGIC_ENUM_INCLUDE} -I${CATCH_INCLUDE}
	LINK = -ljsoncpp ${VULKAN_LIB}/vulkan-1.lib ${GLFW_LIB}/glfw3.dll ${CATCH_HOME}/catch_amalgamated.o
	COMPILE = -std=c++20 -O1 -Wno-deprecated-declarations -Wall -g
	DEFINE = -Duse_validation -Dtests_here
endif

CPP_FILES = $(wildcard */*.cpp) $(wildcard */*/*.cpp)
TO_TEMP = $(addprefix temp/,$(subst code-,,$(subst /,-,$(1))))
O_FILES = $(patsubst %.cpp,%.o,$(call TO_TEMP,${CPP_FILES}))
D_FILES = $(patsubst %.cpp,%.d,$(call TO_TEMP,${CPP_FILES}))

GLSL_FILES = $(wildcard */*/*.vert) $(wildcard */*/*.frag)
SPV_FILES = $(addprefix build/shaders/,$(addsuffix .spv,$(subst .,-,$(subst -shader,,$(subst VulkanCore-,,$(subst /,-,${GLSL_FILES}))))))

build/main.exe : ${D_FILES} ${O_FILES} ${SPV_FILES}
	g++ ${COMPILE} ${DEFINE} ${O_FILES} ${LINK} -o build/main.exe

define CPP_2_D_RECIPE
$(1)
	g++ -MM $${DEFINE} $$^ > $$@
endef
$(foreach file,$(join $(addsuffix :,$(D_FILES)),$(CPP_FILES)),$(eval $(call CPP_2_D_RECIPE,$(file))))

define CPP_2_O_USING_D_RECIPE
$(patsubst %.d,%.o,$(1)) : $(patsubst \,,$(patsubst %.o:,,$(file < $(1))))
	g++ $${COMPILE} $${DEFINE} $${INCLUDE} -c $$< -o $$@
endef
$(foreach file,$(D_FILES),$(eval $(call CPP_2_O_USING_D_RECIPE,$(file))))

define GLSL_2_SPV_RECIPE
$(1)
	glslc $$^ -o $$@
endef
$(foreach file,$(join $(addsuffix :,$(SPV_FILES)),$(GLSL_FILES)),$(eval $(call GLSL_2_SPV_RECIPE,$(file))))
