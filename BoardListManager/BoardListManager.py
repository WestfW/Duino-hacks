'''
  We can prevent boards from showing up in the "Tools/Boards" menu
  of the Arduino IDE by setting boardname.hide=true in the relevant
  boards.txt file.  This program scans all (?) of the boards.txt files
  that will be used a particular version of the IDE:
     hardware/*/boards.txt
     portable/packages/*/boards.txt
     ~/Documents/Arduino/hardware/boards.txt
     ~/Library/Arduino*/*/boards.txt
  builds a list of all of the individual board names, uses a GUI to
  allow the user to pick which boards should be hidden/shown,
  modifies the files, and (maybe) stores the status in a json file for
  future reference.
'''

import os
import json
import tkinter

'''
Our big data structure is essentially a dictionary that maps a list of
found boards.txt file paths to a list of board names within that file
(also a dictionary), which in turn map to whether the board is hidden or not.
We wrap classes around the dictionaries so that we can easily store
additional information, and access things conveniently.
'''

class board():
    def __init__(self,tag,name, hide, bf):
        self.tag = tag
        self.name= name
        self.hide= hide
        self.boardfile = bf
    def setChanged(self):
        self.boardfile.changes = True
    def toButton(self, win):
        self.var =  tkinter.IntVar()
        self.var.set(self.hide)
        tup = (self, tkinter.Checkbutton(win, text=self.name, variable=self.var, command=self.setChanged))
        win.window_create(tkinter.END, window=tup[1])
        win.insert(tkinter.END, "\n")
        return tup

class boardFile():
    """ maintain a list list of boards from a particular boards.txt """
    def __init__(self, pn):
        self.boards = {}
        self.platformName = pn
        self.changes = False
        #print("creating boardlist for pn="+pn)
    def addBoard(self, bname, val, tag, fn):
        b=board(tag, bname, val, self)
        self.boards[tag] = b
    def getBoardNames(self):
        return {self.boards[k].name:self.boards[k].hide for k in self.boards.keys() }
    def getBoardTags(self):
        return {k:self.boards[k].hide for k in self.boards.keys() }


class fileList():
    """ dictionary mapping filesnames to board lists """
    def __init__(self):
        self.files = {}
    def addBoardToFile(self, filename, boardname, val):
        if not filename in self.files:
            self.files[filename] = boardFile()
        self.files[filename].addboard(boardname, val)
    def addFilePath(self, filepath, pn):
        self.files[filepath] = boardFile(pn)
    def getFiles(self):
        return self.files
    def getBoardList(self,filename):
        return(self.files[filename])
    def toJSON(self):
        result = {}
        for file,boardlist in self.files.items():
            result[file] = boardlist.getBoardNames()
        return result
    def toTK(self, win, hideoff):
        '''
        Given the list of board types and locations (from findBoardNames),
        put the list in the Tk window.   The window needs to be a text widget,
        so that it can be scrolled.
        '''
        self.buttons=[]
        win.tag_config("label", background="pink")
        for tag,boards in self.files.items():
            win.insert(tkinter.END, "....."+boards.platformName+".....\n", ("label"))
            for xtag,brd in boards.boards.items():
                if hideoff == 0 or brd.hide == 0:
                    self.buttons.append(brd.toButton(win))
        b = tkinter.Button(win, text="Preview", command=previewCallback)
        c = tkinter.Button(win, text="Done", command=PgmQuit)
        d = tkinter.Button(win, text="Abort", command=PgmAbort)
        win.window_create(tkinter.END, window=b)
        win.window_create(tkinter.END, window=c)
        win.window_create(tkinter.END, window=d)
        return self.buttons
    def reWrite(self):
        for filename, f in self.files.items():
            if f.changes:
                print("........."+filename)
                rewriteBoardFile(filename, f)
            

FILELIST = fileList()

def findBoardFiles(path):
  '''
  Given a top-level path, walk the file system and find all of the
  "boards.txt" files that are underneath.  Put them in a dictionary
  FILELIST that is json-dumpable.
  '''
  for (dirpath, dirnames, filenames) in os.walk(path):
    # print(dirpath)
    if "boards.txt" in filenames:
      if "platform.txt" in filenames:
          platformName = ""
          F = open(dirpath+"/platform.txt")
          for line in F:
              if "name=" == line[:5]:
                  platformName = line[5:-1]
                  break
          F.close()
      FILELIST.addFilePath(dirpath+"/boards.txt", platformName)
      
  return FILELIST

def findBoardNames(pathlist):
  '''FILE
  Given a list of boards.txt filenames, read each file and extract all the
  Arduino "boards" defined within the file.  add sub-dictionaries to the
  FILELIST dictionary.  Note that it remains json-dumpable.
  '''
  for filename in FILELIST.getFiles():
    print("parsing: "+filename)
    F = open(filename)
    blist = FILELIST.getBoardList(filename)
    hidden={}
    for line in F:
      print(line)
      if ".hide=" in line:
          if line.endswith("true\n"):
              hidden[line[0:line.index(".hide")]] = 1
          else:
              hidden[line[0:line.index(".hide")]] = 0
      if ".name=" in line:
#       addBoardtoFile(filename, line[line.index("=")+1:-1], 0)
        tag = line[0:line.index(".name")]
        if tag in hidden:
            hide = hidden[tag]
        else:
            hide = 0
        blist.addBoard(line[line.index("=")+1:-1], hide, tag, blist)
    F.close()
  return FILELIST

def printBoardNames(names):
  '''
  Given the list of board types and locations (from findBoardNames),
  pretty-print the list for human consumption
  '''
  for bn in names:
    desc = bn[bn.index("=")+1:-1]
    tag = bn[bn.index("/ ")+2:bn.index(".name=")]
    print(tag+": "+desc)


def rewriteBoardFile(name, bf):
    '''
    Rename the existing boardfile to a backup, then re-write the file
    with the new hidden board lines (being careful to replace any
    existing <board>.hide lines.)  Note that the .hide lines do not need
    to be near the rest of the symbols defined for the board; we put them
    all at the top of the file.
    '''
    if not bf.changes:
        return
    oldfile = name+".bak"
    os.replace(name, oldfile)
    new = open(name, "w")
    bt = bf.getBoardTags()
    for board, hide in bt.items():
        if hide:
            new.write(board+".hide=true\r\n")
            print(board+".hide=true")
    F = open(oldfile)
    for line in F:
        if not (".hide=" in line):  # remove old .hide lines
            new.write(line)
    F.close()
    new.close()


# for (p, ds, fs) in os.walk(os.environ['HOME']+"/Documents/Classes"):

# b1 = findBoards(os.environ['HOME']+"/Documents/Arduino/")
b = findBoardFiles("//Volumes/2TB/Arduino-test.app/Contents/Java")
n = findBoardNames(b)


rootWin = tkinter.Tk()
rootWin.title("Board Menu Manager")

vsb = tkinter.Scrollbar(orient="vertical")
text = tkinter.Text(rootWin, width=70, height=50, yscrollcommand=vsb.set)
vsb.config(command=text.yview)
vsb.pack(side="right",fill="y")
text.pack(side="top",fill="both",expand=True)

def PgmQuit():
    global buttons, vsb, FILELIST
    for b in buttons:
        board = b[0]
#       tag = b[2]
        if board.var.get() == 1:
            board.hide = 1
    FILELIST.reWrite()
    vsb.quit()
    rootWin.quit()
    rootWin.destroy()
    
def PgmAbort():
    vsb.quit()
    rootWin.quit()
    rootWin.destroy()

def previewCallback():
  global buttons
  window = tkinter.Toplevel()
  window.title("Enabled Boards Preview")
  lastplatform = ""
  for b in buttons:
      board = b[0]
#      tag = b[2]
      boardfile = board.boardfile
      if boardfile.platformName != lastplatform:
          t= tkinter.Text(window, height=1, bg="pink")
          t.pack()
          t.insert(tkinter.END, "....."+boardfile.platformName+".....\n")
          lastplatform = boardfile.platformName
      if board.var.get() == 0:
          c = tkinter.Checkbutton(window, text=board.name)
          c.pack(anchor="w")
      else:
          board.hide = 1
      print("Click")


buttons = FILELIST.toTK(text, 0)

print(json.dumps(FILELIST.toJSON(),indent=2))

rootWin.mainloop()

print("I guess we're done")
# FILELIST.reWrite()
