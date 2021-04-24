

YELLOW=\033[1;33m
GREEN=\033[1;32m
RESET=\033[0m

CC=gcc
CXX=g++
FLAGS=

SRC_DIR=src
INC_DIR=include
OBJ_DIR=objs
DEP_DIR=deps


SRCS=$(wildcard $(SRC_DIR)/*)
DEPS=$(patsubst $(SRC_DIR)/%.c, $(DEP_DIR)/%.d, $(SRCS))
OBJS=$(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
MAIN=client
TEST=test
INCLUDE=$(wildcard $(INC_DIR)/*)


$(MAIN).out: $(OBJ_DIR)/$(MAIN).o $(OBJS)
	@echo "${YELLOW}----- Compiling $@ -----------${RESET}"
	@$(CC) $^ -o $@
	@echo "${GREEN}------- SUCCEED ---------------${RESET}" 



######## CREATE DIRECTORIES ################
# create the directories if don't exist
$(OBJS): | $(OBJ_DIR)
$(OBJ_DIR)/$(TEST).o: | $(OBJ_DIR)
$(OBJ_DIR)/$(MAIN).o: | (OBJ_DIR)

$(DEPS) : | $(DEP_DIR)
$(DEP_DIR)/$(MAIN).d: | $(DEP_DIR)
$(DEP_DIR)/$(TEST).d: | $(DEP_DIR)

$(OBJ_DIR): 
	mkdir $(OBJ_DIR)

$(DEP_DIR):
	mkdir $(DEP_DIR)


-include $(DEPS)
-include $(DEP_DIR)/$(MAIN).d
-include $(DEP_DIR)/$(TEST).d

############# DEPENDENCY MAKEFILES #############
$(DEP_DIR)/%.d: $(SRC_DIR)/%.c
	@echo "----- Creating makefile for $@ ------------"
	$(CC) $(FLAGS) -I$(INC_DIR) -MM -MT $(patsubst $(DEP_DIR)/%.d, $(OBJ_DIR)/%.o, $@) $< > $@


$(DEP_DIR)/$(MAIN).d: $(MAIN).c
	@echo "----- Creating makefile for $@ ------------"
	$(CC) $(FLAGS) -I$(INC_DIR) -MM -MT $(OBJ_DIR)/$(MAIN).o $< > $@

$(DEP_DIR)/$(TEST).d: $(TEST).c
	@echo "----- Creating makefile for $@ ------------"
	$(CC) $(FLAGS) -I$(INC_DIR) -MM -MT $(OBJ_DIR)/$(TEST).o $< > $@


########## COMPILATION ################
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "${YELLOW}----- Compiling $@ -----------${RESET}"
	@$(CC) $(FLAGS) -I$(INC_DIR) -c -o $@ $<
	@echo "${GREEN}------- SUCCEED ---------------${RESET}" 

$(OBJ_DIR)/$(MAIN).o: $(MAIN).c
	@echo "${YELLOW}----- Compiling $@ -----------${RESET}"
	@$(CC) $(FLAGS) -I$(INC_DIR) -c -o $@ $<
	@echo "${GREEN}------- SUCCEED ---------------${RESET}" 

$(OBJ_DIR)/$(TEST).o: $(TEST).c
	@echo "${YELLOW}----- Compiling $@ -----------${RESET}"
	@$(CC) $(FLAGS) -I$(INC_DIR) -c -o $@ $<
	@echo "${GREEN}------- SUCCEED ---------------${RESET}" 


############ TEST  ############
.PHONY: test

test: $(TEST).out
	./$(TEST).out

$(TEST).out: $(OBJ_DIR)/$(TEST).o $(OBJS)
	@echo "${YELLOW}----- Compiling $@ -----------${RESET}"
	@$(CC) $^ -o $@
	@echo "${GREEN}------- SUCCEED ---------------${RESET}" 


	
.PHONY: clean
clean:
	rm -rf *.out $(OBJ_DIR) $(DEP_DIR)
	
