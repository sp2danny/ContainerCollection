##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=ContainerCollection
ConfigurationName      :=Debug
WorkspacePath          :=/home/daniel/project/ContainerCollection
ProjectPath            :=/home/daniel/project/ContainerCollection
IntermediateDirectory  :=./bin/Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=daniel
Date                   :=31/10/18
CodeLitePath           :=/home/daniel/.codelite
LinkerName             :=/usr/bin/clang++
SharedObjectLinkerName :=/usr/bin/clang++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=./test.out
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="ContainerCollection.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -O0
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/clang++
CC       := /usr/bin/clang
CXXFLAGS :=  -g -Wall -Wextra -Werror -pedantic -std=c++17 $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./bin/Debug || $(MakeDirCommand) ./bin/Debug


$(IntermediateDirectory)/.d:
	@test -d ./bin/Debug || $(MakeDirCommand) ./bin/Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix): src/test_all.cpp $(IntermediateDirectory)/src_test_all.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/daniel/project/ContainerCollection/src/test_all.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_test_all.cpp$(DependSuffix): src/test_all.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_test_all.cpp$(DependSuffix) -MM src/test_all.cpp

$(IntermediateDirectory)/src_test_all.cpp$(PreprocessSuffix): src/test_all.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_test_all.cpp$(PreprocessSuffix) src/test_all.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./bin/Debug/


