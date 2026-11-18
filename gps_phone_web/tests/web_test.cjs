const fs=require('node:fs');const vm=require('node:vm');const assert=require('node:assert/strict');
const html=fs.readFileSync(__dirname+'/../web/index.html','utf8');const js=html.match(/<script>([\s\S]*?)<\/script>/)[1];
function load(search){const nodes={};const document={getElementById(id){return nodes[id]??=( {textContent:'',disabled:false,hidden:true,attributes:{},setAttribute(k,v){this.attributes[k]=v},removeAttribute(k){delete this[k]},addEventListener(){}} )}};const context=vm.createContext({document,URLSearchParams,location:{search},navigator:{},window:{},Number,AbortSignal,setTimeout:()=>{},fetch:async()=>{throw Error('offline')}});vm.runInContext(js,context);return {nodes,context}}
let {nodes,context}=load('?demo=1');assert.equal(nodes.lat.textContent,'1.352083');assert.match(nodes.map.href,/maps\/search/);assert.equal(nodes.copy.disabled,false);assert.equal(nodes.demo.hidden,false);
vm.runInContext('disconnected()',context);assert.equal(nodes.lat.textContent,'—');assert.equal(nodes.copy.disabled,true);assert.equal(nodes.map.href,undefined);
({nodes}=load('?demo=1&state=nofix'));assert.equal(nodes.lat.textContent,'—');assert.equal(nodes.satellites.textContent,0);assert.equal(nodes.copy.disabled,true);assert.match(nodes.status.textContent,/no fix/);
({nodes}=load('?demo=1&state=disconnected'));assert.match(nodes.status.textContent,/disconnected/);
console.log('Web tests passed: live readings, no-fix, disconnect, coordinate actions, labeled demo.');
