# Variables
CC = gcc -g -Wall -pedantic
CFLAGS = -g
CPPFLAGS = 
LDLIBS = -lm 
BIBLI=-lgraphe -lliste
# gestion du répertoire ./src
SRCDIR = ./SRC
# gestion du répertoire ./obj
OBJDIR = ./OBJ
# gestion du répertoire ./bin
BINDIR = ./BIN
# gestion du répéertoire ./lib
LIBDIR = ./LIB

# Variables propres a cette applie
MAIN = $(BINDIR)/main
LIB = $(LIBDIR)/libgraphe.a $(LIBDIR)/libliste.a
SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

# cible principale
all : $(MAIN)

# Regle principale pour l'edition des liens
$(MAIN) : $(LIBDIR) $(LIB) $(BINDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) ./OBJ/main.o -I $(SRCDIR) -L $(LIBDIR) $(BIBLI) -o $(MAIN)

# Regle principale avec l'edition de lien de notre librairie
$(LIB) : $(OBJ)
	ar r $@ $^
	ranlib $@

# Regle de construction des .o
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(SRCDIR)/%.h $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $(OBJDIR)/$*.o 

# Regle de construction des .o
$(OBJDIR)/%.o : $(SRCDIR)/%.c $(OBJDIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $(OBJDIR)/$*.o 

$(OBJDIR) : 
	mkdir $(OBJDIR)

$(BINDIR) : 
	mkdir $(BINDIR)

$(LIBDIR) :
	mkdir $(LIBDIR)

essai : 
	@echo $(SRC) ;
	@echo $(OBJ)

# Nettoyage des fichiers intermediaires issu de la compilation
clean : 
	rm $(OBJDIR)/*.o
	rm $(LIB) ;
	rm $(MAIN) ;
	rmdir $(LIBDIR) ;
	rmdir $(OBJDIR) ;
	rmdir $(BINDIR)
