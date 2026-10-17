"""Match FM facts to a 3.9.68 Update Team template: nations, clubs, people. Writes <work>/match.pkl.
usage: python3 -I match.py <season start year> <template folder> <facts folder> <work folder>
The facts folder holds the pickles of the FM reader (nations, clubs, people_all). The template's birth shift comes
from the date shift research (season settings name the template)."""
import os,sys,struct,pickle,unicodedata,collections,re
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from cmdb import *
from seasons import settings,template_shifts
S_=settings(int(sys.argv[1]))
T=Folder(sys.argv[2]); X=os.path.join(sys.argv[3],''); W=sys.argv[4]
SHIFT=template_shifts(S_.TEMPLATE)['birth']
TRANS=str.maketrans({'æ':'ae','Æ':'Ae','ø':'o','Ø':'O','ß':'ss','ð':'d','þ':'th','ı':'i','ł':'l','Ł':'L','đ':'d','Đ':'D'})
def norm(s):
    s=unicodedata.normalize('NFKD',(s or '').translate(TRANS)).encode('ascii','ignore').decode().lower()
    s=re.sub(r'[^a-z0-9 ]',' ',s); return ' '.join(s.split())
STOP={'fc','f c','cf','c f','sc','ac','afc','cd','sd','ud','cfr','fk','sk','if','bk','sv','vfb','vfl','tsv','club','de','the','s a d','sad','calcio','football','futebol','clube'}
def ckey(s):
    w=[x for x in norm(s).split() if x not in STOP and (len(x)>1 or x in ('b','c'))]
    return ' '.join(w)
# nations
tn={struct.unpack_from('<i',r,0)[0]:cstr(r,4,51) for r in T.rows(4)}
fn=pickle.load(open(X+'nations.pkl','rb'))
ALIAS={'holland':'netherlands','the congo':'congo','fyr of macedonia':'macedonia','ivory coast':'cote d ivoire','the philippines':'philippines','republic of ireland':'ireland','china pr':'china','korea republic':'south korea','dr congo':'congo dr','usa':'united states','bosnia':'bosnia herzegovina','serbia mont':'yugoslavia','republic of china':'chinese taipei','serbia':'yugoslavia','montenegro':'yugoslavia','hong kong china pr':'hong kong','macau china pr':'macau','chinese taipei china pr':'chinese taipei','burma':'myanmar','st vincent':'st vincent the grenadines'}
def nk(s):
    k=norm(s); return ALIAS.get(k,k)
tbyname={nk(v):k for k,v in tn.items()}
print('template nation names sample',sorted(tn.values())[:0])
natmap={}
for k,(lng,sh) in fn.items():
    for s in (lng,sh):
        if nk(s) in tbyname: natmap[k]=tbyname[nk(s)]; break
print('nations mapped',len(natmap),'of',len(fn),'unmapped',[fn[k][1] for k in fn if k not in natmap][:40])
# clubs
S,tclubs=staff_view(T)
trow={struct.unpack_from('<i',r,0)[0]:r for r in T.rows(0)}
tc=collections.defaultdict(list)
for cid,r in trow.items():
    nat=struct.unpack_from('<i',r,0x53)[0]
    for s in (cstr(r,4,51),cstr(r,0x38,26)):
        tc[(nat,norm(s))].append(cid); tc[(nat,'k:'+ckey(s))].append(cid); tc[(nat,'z:'+norm(s).replace(' ',''))].append(cid)
fc=pickle.load(open(X+'clubs.pkl','rb'))
tlong=collections.defaultdict(list); tany=collections.defaultdict(list)
for cid,r in trow.items():
    nat=struct.unpack_from('<i',r,0x53)[0]
    tlong[(nat,norm(cstr(r,4,51)))].append(cid)
    for s_ in (cstr(r,4,51),cstr(r,0x38,26)): tany[norm(s_)].append((nat,cid))
best={}   # template club -> (score, fm club)
for cid,c in fc.items():
    nat=natmap.get(c['nat'])
    cands=[]
    if nat is not None:
        for key,sc in ((norm(c['long']),3),):
            for t in tlong.get((nat,key),[]): cands.append((sc,t))
        for key,sc in ((norm(c['long']),2),(norm(c['short']),2),('z:'+norm(c['long']).replace(' ',''),2),('k:'+ckey(c['long']),1),('k:'+ckey(c['short']),1)):
            for t in tc.get((nat,key),[]): cands.append((sc,t))
    else:
        for key in (norm(c['long']),norm(c['short'])):
            hits={t for n_,t in tany.get(key,[])}
            if len(hits)==1: cands.append((1,hits.pop()))
    if not cands: continue
    toks=set(norm(c['long']).split())
    if not toks & {'b','c','ii','iii','reserves','reserve','u19','u21','u23','juniors','youth','sub'}:
        cands=[(sc+0.5,t) for sc,t in cands]
    top=max(sc for sc,_ in cands); ts={t for sc,t in cands if sc==top}
    if len(ts)!=1: continue
    t=ts.pop()
    if t not in best or top>best[t][0]: best[t]=(top,cid,False)
    elif top==best[t][0]: best[t]=(top,best[t][1],True)
clubmap={cid:t for t,(sc,cid,tie) in best.items() if not tie}
dup=[t for t,v in best.items() if v[2]]
print('clubs mapped',len(clubmap),'of',len(fc),'(dropped',len(dup),'template clubs hit twice)')
# people
fp=pickle.load(open(X+'people_all.pkl','rb'))
import datetime
CUM=[0,31,59,90,120,151,181,212,243,273,304,334]
def cm_md(d):
    # CM day of year uses the common-year table
    m=max(i for i in range(12) if CUM[i]<=d); return (m+1,d-CUM[m]+1)
def pkeys(first,second,common,dob):
    out=[]
    if dob: 
        out.append((norm(first),norm(second),dob))
        if common: out.append(('c',norm(common),dob))
    return out
tidx=collections.defaultdict(list); sidx=collections.defaultdict(list)
for x in S:
    d,y=x['dob']
    if y<1901: continue
    dob=(cm_md(d),y+SHIFT)
    for k in pkeys(x['first'],x['second'],x['common'],dob): tidx[k].append(x['id'])
    sidx[(norm(x['second']).split()[-1] if norm(x['second']) else '',dob)].append(x['id'])
    if x['common']: sidx[(norm(x['common']).split()[-1],dob)].append(x['id'])
nidx=collections.defaultdict(list)
for x in S:
    d,y=x['dob']
    if y<1901: continue
    for k in ((norm(x['first']),norm(x['second'])),('c',norm(x['common'])) if x['common'] else None):
        if k: nidx[k].append((x['id'],y+SHIFT,x['nation']))
pmap={}; amb=0
for i,x in enumerate(fp):
    if not x['dob'] or x['dob'].year<=1900: continue
    dob=((x['dob'].month,x['dob'].day),x['dob'].year)
    cands=set(); tier=1
    for k in pkeys(x['fn'],x['sn'],x['cn'],dob): 
        if k in tidx: cands|=set(tidx[k])
    if not cands and x['full']:
        parts=norm(x['full']).split()
        if len(parts)>=2:
            for k in (('c',norm(x['full']),dob),(parts[0],' '.join(parts[1:]),dob),(norm(x['fn']),parts[-1],dob)):
                if k in tidx: cands|=set(tidx[k])
    if not cands:
        tier=2
        sn=norm(x['sn']).split(); cn=norm(x['cn']).split()
        for last in {sn[-1] if sn else '', cn[-1] if cn else ''}-{''}:
            for dd in (0,-1,1):
                try: dt=x['dob']+datetime.timedelta(days=dd)
                except: continue
                k=(last,((dt.month,dt.day),dt.year))
                if k in sidx and len(sidx[k])==1: cands|=set(sidx[k])
            if cands: break
    if not cands:
        tier=3
        tn_=natmap.get(x['nat'])
        for k in ((norm(x['fn']),norm(x['sn'])),('c',norm(x['cn'])) if x['cn'] else None):
            if not k: continue
            c2={tid for tid,ty,tnat in nidx.get(k,[]) if abs(ty-x['dob'].year)<=1 and (tn_ is None or tnat==tn_)}
            if len(c2)==1: cands=c2; break
    if not cands:
        tier=4
        for k in ((norm(x['fn']),norm(x['sn'])),('c',norm(x['cn'])) if x['cn'] else None):
            if not k: continue
            allc=nidx.get(k,[])
            if len(allc)==1:
                tid,ty,tnat=allc[0]; fy=x['dob'].year
                if abs(ty-fy)<=2 or abs(ty-SHIFT-fy)<=1: cands={tid}; break
    if len(cands)==1: pmap[i]=(cands.pop(),tier,0 if x['club'] is not None else 1)
    elif len(cands)>1: amb+=1
bestp={}
for i,(t,tier,noclub) in pmap.items():
    sc=(tier,noclub)
    if t not in bestp or sc<bestp[t][0]: bestp[t]=(sc,i,False)
    elif sc==bestp[t][0]: bestp[t]=(sc,bestp[t][1],True)
pmap={i:t for t,(sc,i,tie) in bestp.items() if not tie}
print('people mapped',len(pmap),'ambiguous',amb)
# clubs that the names did not map: the template club of most of their matched players (same nation)
tclub_of={x['id']:x['club'] for x in S}
tnat={cid:struct.unpack_from('<i',r,0x53)[0] for cid,r in trow.items()}
votes=collections.defaultdict(collections.Counter)
for i,t in pmap.items():
    fc_=fp[i]['club']
    if fc_ is not None and fc_ in fc and fc_ not in clubmap and fp[i]['role']=='player':
        c=tclub_of.get(t,-1)
        if c>=0: votes[fc_][c]+=1
taken=set(clubmap.values()); added=0
for fc_,cnt in sorted(votes.items(),key=lambda kv:-sum(kv[1].values())):
    c,n=cnt.most_common(1)[0]; tot=sum(cnt.values())
    nat=natmap.get(fc[fc_]['nat'])
    if n>=5 and n>=0.4*tot and c not in taken and (nat is None or tnat.get(c)==nat):
        clubmap[fc_]=c; taken.add(c); added+=1
print('clubs mapped by their players',added,'total',len(clubmap))
# Fixed pairs of the season settings (FM long name -> template long name), where the names do not match.
tlong_ids=collections.defaultdict(list)
for cid,r in trow.items(): tlong_ids[cstr(r,4,51)].append(cid)
fby=collections.defaultdict(list)
for cid,c in fc.items(): fby[c['long']].append(cid)
for a,b in getattr(S_,'CLUB_FIX',{}).items():
    if len(fby[a])!=1 or len(tlong_ids[b])!=1: sys.exit(f'match: CLUB_FIX {a} -> {b}: {len(fby[a])} FM clubs, {len(tlong_ids[b])} template clubs')
    t=tlong_ids[b][0]
    for k in [k for k,v in clubmap.items() if v==t]: del clubmap[k]
    clubmap[fby[a][0]]=t
print('clubs fixed by the season settings',len(getattr(S_,'CLUB_FIX',{})))
pickle.dump(dict(natmap=natmap,clubmap=clubmap,pmap=pmap),open(os.path.join(W,'match.pkl'),'wb'))
# coverage for clubs in divisions (playable) in the template
play={cid for cid,r in trow.items() if struct.unpack_from('<i',r,0x57)[0]>=0}
inv={v:k for k,v in clubmap.items()}
print('template league clubs',len(play),'mapped from FM',sum(1 for c in play if c in inv))
fpl=[(i,x) for i,x in enumerate(fp) if x['role']=='player' and x['club'] in clubmap and clubmap[x['club']] in play]
print('FM players at template league clubs',len(fpl),'matched',sum(1 for i,_ in fpl if i in pmap))

# per top league coverage
comps={struct.unpack_from('<i',r,0)[0]:cstr(r,4,51) for r in T.rows(12)}
TOP=['English Premier Division','English First Division','Italian Serie A','Spanish First Division','German First Division','French First Division','Dutch Eredivisie','Portuguese Premier League','Scottish Premier Division','Belgian First Division','Turkish First Division','Brazilian National First Division','Argentine Premier Division']
tid={v:k for k,v in comps.items()}
for t in TOP:
    if t not in tid: print('no comp',t); continue
    cl={c for c,r in trow.items() if struct.unpack_from('<i',r,0x57)[0]==tid[t]}
    fpl=[(i,x) for i,x in enumerate(fp) if x['role']=='player' and x['club'] in clubmap and clubmap[x['club']] in cl]
    print('%-34s clubs %3d mapped %3d  FM players %5d matched %5d'%(t,len(cl),sum(1 for c in cl if c in inv),len(fpl),sum(1 for i,_ in fpl if i in pmap)))
for t in TOP:
    if t not in tid: continue
    cl=[c for c,r in trow.items() if struct.unpack_from('<i',r,0x57)[0]==tid[t] and c not in inv]
    print(t,'unmapped:',[(cstr(trow[c],4,51),cstr(trow[c],0x38,26)) for c in cl])
