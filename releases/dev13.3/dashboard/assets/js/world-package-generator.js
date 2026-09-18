'use strict';
(() => {
  const form=document.getElementById('world-generator-form');
  if(!form)return;
  const state=document.getElementById('world-generator-state');
  const checks=document.getElementById('world-generator-checks');
  const log=document.getElementById('world-generator-log');
  const validateBtn=document.getElementById('world-generator-validate');
  const nextBtn=document.getElementById('world-builder-next');
  const backBtn=document.getElementById('world-builder-back');
  const progress=document.getElementById('world-builder-progress');
  const endpoint='api/world-generator.php';
  const steps=[...document.querySelectorAll('[data-world-builder-step]')];
  let step=1, validation=null, generated=null;
  const payload=()=>Object.fromEntries(new FormData(form).entries());
  const setState=(label,kind='')=>{state.textContent=label;state.className=`status-pill ${kind}`.trim()};
  const writeLog=(lines)=>{if(log)log.textContent=(Array.isArray(lines)?lines:[lines]).join('\n')};
  const esc=s=>String(s??'').replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
  function renderProgress(){
    progress.innerHTML=['World','Source','Validate','Generate','Complete'].map((label,i)=>`<div class="${i+1<step?'done':i+1===step?'active':''}"><span>${i+1<step?'✓':i+1}</span><small>${label}</small></div>`).join('');
    steps.forEach(x=>x.hidden=Number(x.dataset.worldBuilderStep)!==step);
    backBtn.hidden=step===1||step===5;
    nextBtn.hidden=step>=4;
    nextBtn.textContent=step===3?'Continue to generation →':'Continue →';
  }
  function updateSourceFolder(){
    const id=form.elements.id.value.trim();
    form.elements.source_folder.value=`map-tools/import/${id}`;
  }
  function renderChecks(result){
    const list=result?.checks||[];
    checks.innerHTML=list.map(x=>`<div class="${x.ok?'ok':'warn'}"><span>${x.ok?'✓':'!'}</span><div><strong>${esc(x.label)}</strong><small>${esc(x.detail||'')}</small></div></div>`).join('')||'<p class="muted">No checks returned.</p>';
    const passed=list.filter(x=>x.ok).length, total=Math.max(1,list.length), percent=Math.round(passed/total*100);
    document.getElementById('world-validation-percent').textContent=`${percent}%`;
    document.querySelector('.world-progress-ring')?.style.setProperty('--world-progress',`${percent*3.6}deg`);
    document.getElementById('world-validation-title').textContent=result.ready?'Ready to generate':'Source incomplete';
    document.getElementById('world-validation-copy').textContent=result.ready?'All required checks passed. Optional empty layers can still be generated.':'Add the missing required assets, then scan again.';
  }
  async function request(action){
    const body=new URLSearchParams({action,...payload()});
    const response=await fetch(endpoint,{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded;charset=UTF-8'},body});
    const data=await response.json();
    if(!response.ok||data.ok===false)throw new Error(data.error?.message||'World builder request failed');
    return data.data||{};
  }
  async function validate(){
    setState('SCANNING');
    try{validation=await request('validate');renderChecks(validation);writeLog(validation.log||['Validation complete.']);setState(validation.ready?'READY':'INCOMPLETE',validation.ready?'online':'offline');return validation}catch(e){setState('ERROR','offline');writeLog(e.message);throw e}
  }
  function animateGeneration(){
    const bar=document.getElementById('world-generation-bar');let p=8;bar.style.width=`${p}%`;
    const timer=setInterval(()=>{p=Math.min(92,p+Math.random()*13);bar.style.width=`${p}%`},180);return()=>{clearInterval(timer);bar.style.width='100%'};
  }
  async function generate(){
    if(!validation?.ready){validation=await validate();if(!validation.ready){step=3;renderProgress();return}}
    setState('GENERATING');writeLog(['Preparing package directory…','Normalising vector layers…','Building manifest and tile metadata…']);
    const finish=animateGeneration();
    try{
      generated=await request('generate');finish();renderChecks(generated);writeLog(generated.log||['Generation complete.']);setState('GENERATED','online');
      document.getElementById('world-result-name').textContent=generated.package_name||`${form.elements.id.value}.swp`;
      document.getElementById('world-result-checksum').textContent=(generated.checksum||'Not recorded').slice(0,18)+(generated.checksum?.length>18?'…':'');
      document.getElementById('world-result-checksum').title=generated.checksum||'';
      document.getElementById('world-result-signature').textContent=generated.signature?.status||'UNSIGNED';
      document.getElementById('world-result-path').textContent=generated.path||'—';
      document.getElementById('world-builder-success-copy').textContent=`${form.elements.name.value} is registered and ready for installation.`;
      document.getElementById('maps-refresh')?.click();step=5;renderProgress();
    }catch(e){finish();setState('ERROR','offline');writeLog(e.message)}
  }
  document.querySelectorAll('[data-world-preset]').forEach(button=>button.addEventListener('click',()=>{
    document.querySelectorAll('[data-world-preset]').forEach(x=>x.classList.toggle('active',x===button));
    const custom=button.dataset.worldPreset==='custom';
    form.elements.id.value=custom?'custom-world':button.dataset.worldPreset;
    form.elements.name.value=button.dataset.name;
    form.elements.world_size.value=button.dataset.size;
    form.elements.origin.value=button.dataset.origin;
    updateSourceFolder();validation=null;
  }));
  document.querySelectorAll('.world-source-card input').forEach(r=>r.addEventListener('change',()=>document.querySelectorAll('.world-source-card').forEach(x=>x.classList.toggle('active',x.querySelector('input').checked))));
  form.elements.id.addEventListener('input',()=>{updateSourceFolder();validation=null});
  validateBtn.addEventListener('click',()=>validate().catch(console.error));
  nextBtn.addEventListener('click',async()=>{
    if(step===1&&(!form.reportValidity()))return;
    if(step===3){const v=await validate().catch(()=>null);if(!v?.ready)return;}
    step=Math.min(4,step+1);renderProgress();
  });
  backBtn.addEventListener('click',()=>{step=Math.max(1,step-1);renderProgress()});
  form.addEventListener('submit',e=>{e.preventDefault();generate()});
  document.getElementById('world-builder-open-library')?.addEventListener('click',()=>document.getElementById('maps-refresh')?.scrollIntoView({behavior:'smooth',block:'center'}));
  document.getElementById('world-builder-open-world')?.addEventListener('click',()=>{document.querySelector('[data-view="map"]')?.click()});
  updateSourceFolder();renderProgress();
})();
