.PHONY: clean All

All:
	@echo "----------Building project:[ ContainerCollection - Debug ]----------"
	@$(MAKE) -f  "ContainerCollection.mk"
clean:
	@echo "----------Cleaning project:[ ContainerCollection - Debug ]----------"
	@$(MAKE) -f  "ContainerCollection.mk" clean
