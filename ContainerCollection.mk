##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Release
ProjectName            :=ContainerCollection
ConfigurationName      :=Release
WorkspacePath          := "/home/sp2danny/extra/ContainerCollection"
ProjectPath            := "/home/sp2danny/extra/ContainerCollection"
IntermediateDirectory  :=./bin/Release
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Daniel Nyström
Date                   :=05/09/19
CodeLitePath           :="/home/sp2danny/.codelite"
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
LinkOptions            :=  -O3
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)pthread 
ArLibs                 :=  "pthread" 
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Release 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/clang++ 
CC       := /usr/bin/clang 
CXXFLAGS :=  -O3 -DNDEBUG -Wall -Wextra -Werror -pedantic -std=c++17 $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as 


##
## User defined environment variables
##
Objects0=$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_performance_tester.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_test_item.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_container_tester.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_graph.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_test_integrity.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

$(IntermediateDirectory)/.d:
	@test -d ./bin/Release || $(MakeDirCommand) ./bin/Release

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix): src/test_all.cpp $(IntermediateDirectory)/src_test_all.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/test_all.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_test_all.cpp$(DependSuffix): src/test_all.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_test_all.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_test_all.cpp$(DependSuffix) -MM "src/test_all.cpp"

$(IntermediateDirectory)/src_test_all.cpp$(PreprocessSuffix): src/test_all.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_test_all.cpp$(PreprocessSuffix) "src/test_all.cpp"

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp $(IntermediateDirectory)/src_main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(DependSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM "src/main.cpp"

$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) "src/main.cpp"

$(IntermediateDirectory)/src_performance_tester.cpp$(ObjectSuffix): src/performance_tester.cpp $(IntermediateDirectory)/src_performance_tester.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/performance_tester.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_performance_tester.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_performance_tester.cpp$(DependSuffix): src/performance_tester.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_performance_tester.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_performance_tester.cpp$(DependSuffix) -MM "src/performance_tester.cpp"

$(IntermediateDirectory)/src_performance_tester.cpp$(PreprocessSuffix): src/performance_tester.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_performance_tester.cpp$(PreprocessSuffix) "src/performance_tester.cpp"

$(IntermediateDirectory)/src_test_item.cpp$(ObjectSuffix): src/test_item.cpp $(IntermediateDirectory)/src_test_item.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/test_item.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_test_item.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_test_item.cpp$(DependSuffix): src/test_item.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_test_item.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_test_item.cpp$(DependSuffix) -MM "src/test_item.cpp"

$(IntermediateDirectory)/src_test_item.cpp$(PreprocessSuffix): src/test_item.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_test_item.cpp$(PreprocessSuffix) "src/test_item.cpp"

$(IntermediateDirectory)/src_container_tester.cpp$(ObjectSuffix): src/container_tester.cpp $(IntermediateDirectory)/src_container_tester.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/container_tester.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_container_tester.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_container_tester.cpp$(DependSuffix): src/container_tester.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_container_tester.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_container_tester.cpp$(DependSuffix) -MM "src/container_tester.cpp"

$(IntermediateDirectory)/src_container_tester.cpp$(PreprocessSuffix): src/container_tester.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_container_tester.cpp$(PreprocessSuffix) "src/container_tester.cpp"

$(IntermediateDirectory)/src_graph.cpp$(ObjectSuffix): src/graph.cpp $(IntermediateDirectory)/src_graph.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/graph.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_graph.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_graph.cpp$(DependSuffix): src/graph.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_graph.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_graph.cpp$(DependSuffix) -MM "src/graph.cpp"

$(IntermediateDirectory)/src_graph.cpp$(PreprocessSuffix): src/graph.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_graph.cpp$(PreprocessSuffix) "src/graph.cpp"

$(IntermediateDirectory)/src_test_integrity.cpp$(ObjectSuffix): src/test_integrity.cpp $(IntermediateDirectory)/src_test_integrity.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sp2danny/extra/ContainerCollection/src/test_integrity.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_test_integrity.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_test_integrity.cpp$(DependSuffix): src/test_integrity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_test_integrity.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_test_integrity.cpp$(DependSuffix) -MM "src/test_integrity.cpp"

$(IntermediateDirectory)/src_test_integrity.cpp$(PreprocessSuffix): src/test_integrity.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_test_integrity.cpp$(PreprocessSuffix) "src/test_integrity.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) ./bin/Release/*$(ObjectSuffix)
	$(RM) ./bin/Release/*$(DependSuffix)
	$(RM) $(OutputFile)
	$(RM) ".build-release/ContainerCollection"


