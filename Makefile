.PHONY: clean All

All:
	@echo "----------Building project:[ ContainerCollection - Release ]----------"
	@$(MAKE) -f  "ContainerCollection.mk"
clean:
	@echo "----------Cleaning project:[ ContainerCollection - Release ]----------"
	@$(MAKE) -f  "ContainerCollection.mk" clean
