// Usage: node BakeTarotSpine.cjs <original Spine directory> <node_modules directory> <output.json>
// Uses installed @pixi-spine/runtime-3.8 and @pixi/core; does not alter original files.
const fs=require('fs'),path=require('path');
const [dir,deps,output]=process.argv.slice(2);
if(!dir||!deps||!output)throw Error('Expected original directory, dependency directory and output JSON');
const spine=require(path.join(deps,'@pixi-spine/runtime-3.8'));
const base=require(path.join(deps,'@pixi-spine/base')),pixi=require(path.join(deps,'@pixi/core'));
const atlas=new base.TextureAtlas(fs.readFileSync(path.join(dir,'act54side_card_home_bg_char.atlas'),'utf8'),(name,cb)=>cb(pixi.BaseTexture.fromBuffer(new Uint8Array(1800*1608*4),1800,1608)));
const raw=JSON.parse(fs.readFileSync(path.join(dir,'act54side_card_home_bg_char.json'),'utf8'));
const data=new spine.SkeletonJson(new spine.AtlasAttachmentLoader(atlas)).readSkeletonData(raw);
const clips={};
for(const anim of data.animations){
 const skeleton=new spine.Skeleton(data),state=new spine.AnimationState(new spine.AnimationStateData(data));
 const loop=anim.name.startsWith('loop'),count=Math.ceil(anim.duration*30)+(loop?0:1),step=anim.duration/(loop?count:count-1);
 state.setAnimation(0,anim.name,loop);const frames=[];
 for(let f=0;f<count;f++){
  skeleton.setToSetupPose();state.update(f===0?0:step);state.apply(skeleton);skeleton.updateWorldTransform();
  const meshes=[];
  for(const slot of skeleton.drawOrder){
   const a=slot.getAttachment();if(!a||!a.region)continue;
   let positions,uvs,indices;
   if(a instanceof spine.MeshAttachment){
    positions=new Float32Array(a.worldVerticesLength);a.computeWorldVertices(slot,0,a.worldVerticesLength,positions,0,2);
    // TextureMatrix is the same original-size/trim/rotation transform used by SpineMesh's Pixi shader.
    const matrix=new pixi.TextureMatrix(a.region.texture);matrix.update(true);
    uvs=matrix.multiplyUvs(a.regionUVs,new Float32Array(a.regionUVs.length));indices=a.triangles;
   }else if(a instanceof spine.RegionAttachment){
    positions=new Float32Array(8);a.computeWorldVertices(slot.bone,positions,0,2);
    const u=a.region.texture._uvs;uvs=[u.x3,u.y3,u.x0,u.y0,u.x1,u.y1,u.x2,u.y2];indices=[0,1,2,0,2,3];
   }else continue;
   const vertices=[];
   for(let i=0;i<positions.length;i+=2)vertices.push([positions[i],positions[i+1],uvs[i],uvs[i+1]].map(n=>Math.round(n*100000)/100000));
   meshes.push({name:slot.data.name,texture:'act54side_card_home_bg_char',vertices,indices:Array.from(indices),alpha:slot.color.a*a.color.a,color:[slot.color.r*a.color.r,slot.color.g*a.color.g,slot.color.b*a.color.b]});
  }
  frames.push(meshes);
 }
 clips[anim.name]={duration:anim.duration,loop,frames};
 console.log(anim.name,anim.duration,count,frames[0].length);
}
fs.writeFileSync(output,JSON.stringify({version:2,uvMapping:'Pixi TextureMatrix including atlas trim and original size',clips}));
console.log('Wrote',fs.statSync(output).size,'bytes');
