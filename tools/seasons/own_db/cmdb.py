"""Read a CM 01/02 3.9.68 data folder (index.dat, staff.dat tables, names, clubs). Data only."""
import os,struct,re,unicodedata
REC={0:581,1:581,2:58,3:198,4:290,7:43,5:78,6:110,9:68,10:70,11:101,21:56,12:107,16:107,13:60,14:60,15:60,17:17,18:58,19:26,20:26,22:52}
class Folder:
    def __init__(s,path):
        s.path=path; s.raw={}
        for f in os.listdir(path):
            if f.lower().endswith('.dat'): s.raw[f]=bytearray(open(os.path.join(path,f),'rb').read())
        s.reindex()
    def reindex(s):
        ib=s.raw['index.dat']; s.idx={}
        for i in range(8,len(ib)-66,67):
            name=ib[i:i+51].split(b'\0')[0].decode('latin1'); tid,cnt,off,ver=struct.unpack_from('<iiii',ib,i+51)
            s.idx[tid]=dict(file=name,count=cnt,off=off,ver=ver,pos=i)
    def table(s,tid):
        e=s.idx[tid]; size=REC[tid]; b=s.raw[e['file']]
        if tid==6 and e['ver']!=2: raise SystemExit('staff table is not version 2')
        return b,e['off'],size,e['count']
    def rows(s,tid):
        b,off,size,n=s.table(tid)
        return [memoryview(b)[off+i*size:off+(i+1)*size] for i in range(n)]
def cstr(r,o,n): return bytes(r[o:o+n]).split(b'\0')[0].decode('latin1')
def date(r,o):
    d,y=struct.unpack_from('<hH',r,o); return d,y
def names(F,tid):
    return {struct.unpack_from('<i',r,0x33)[0]:cstr(r,0,51) for r in F.rows(tid)}
def staff_view(F):
    fn,sn,cn=names(F,13),names(F,14),names(F,15)
    clubs={struct.unpack_from('<i',r,0)[0]:cstr(r,4,51) for r in F.rows(0)}
    clubs.update({struct.unpack_from('<i',r,0)[0]:cstr(r,4,51) for r in F.rows(1)})
    out=[]
    for r in F.rows(6):
        i,f,s,c=struct.unpack_from('<iiii',r,0)
        out.append(dict(id=i,first=fn.get(f,''),second=sn.get(s,''),common=cn.get(c,'') if c>0 else '',
            dob=date(r,0x10),nation=struct.unpack_from('<i',r,0x1a)[0],club=struct.unpack_from('<i',r,0x39)[0],
            joined=date(r,0x3e),cend=date(r,0x46),kind=r[0x5f],np=struct.unpack_from('<i',r,0x61)[0],pl=struct.unpack_from('<i',r,0x69)[0]))
    return out,clubs
TRANS=str.maketrans({'æ':'ae','Æ':'Ae','ø':'o','Ø':'O','ß':'ss','ð':'d','þ':'th','ı':'i','ł':'l','Ł':'L','đ':'d','Đ':'D'})
def norm(s):
    """A name for matching: letters without accents, lower case, digits, single spaces."""
    s=unicodedata.normalize('NFKD',(s or '').translate(TRANS)).encode('ascii','ignore').decode().lower()
    s=re.sub(r'[^a-z0-9 ]',' ',s); return ' '.join(s.split())
def swords(second,common):
    """Words of a surname and common name (3 letters or more), and each name with its spaces removed."""
    out=set()
    for n in (norm(second),norm(common)):
        out|={w for w in n.split() if len(w)>=3}
        if len(n.replace(' ',''))>=3: out.add(n.replace(' ',''))
    return out
