ts=simGetSimulationTimeStep()

if (simGetScriptExecutionCount()==0) then
	heli=simGetObjectHandle('Quadricopter')
	propeller=simGetObjectHandle('Quadricopter_propeller4')
	propellerRespondable=simGetObjectHandle('Quadricopter_propeller_respondable4')
	type=sim_particle_roughspheres+sim_particle_respondable1to4+sim_particle_respondable5to8+
		sim_particle_cyclic+sim_particle_ignoresgravity
	simulateParticles=simGetScriptSimulationParameter(sim_handle_self,'simulateParticles')
	particleVelocity=simGetScriptSimulationParameter(sim_handle_self,'particleVelocity')

	totalExertedForce=simGetScriptSimulationParameter(sim_handle_self,'totalForce')

	particleCountPerSecond=simGetScriptSimulationParameter(sim_handle_self,'particleCountPerSecond')
	particleDensity=simGetScriptSimulationParameter(sim_handle_self,'particleDensity')
	particleScatteringAngle=simGetScriptSimulationParameter(sim_handle_self,'particleScatteringAngle')
	particleLifeTime=simGetScriptSimulationParameter(sim_handle_self,'particleLifeTime')
	maxParticleCount=simGetScriptSimulationParameter(sim_handle_self,'maxParticleCount')
	if (simGetScriptSimulationParameter(sim_handle_self,'particlesAreVisible')==false) then
		type=type+sim_particle_invisible
	end

	particleObject=nil
	is=0 -- previous size factor
	notFullParticles=0
	params={2,1,0.2,3,0.4}
end

particleVelocity=simGetScriptSimulationParameter(sim_handle_self,'particleVelocity')

totalExertedForce=simGetScriptSimulationParameter(sim_handle_self,'totalForce')

s=simGetObjectSizeFactor(propeller) -- current size factor
if (s~=is) then
	if (particleObject) then
		simRemoveParticleObject(particleObject)
		particleObject=nil
	end
	particleSize=simGetScriptSimulationParameter(sim_handle_self,'particleSize')*0.005*s
	is=s
end

if (particleObject==nil)and(simulateParticles) then
	particleObject=simAddParticleObject(type,particleSize,particleDensity,params,particleLifeTime,maxParticleCount,{0.3,0.7,1})
end

m=simGetObjectMatrix(propeller,-1)
particleCnt=0
pos={0,0,0}
dir={0,0,1}

requiredParticleCnt=particleCountPerSecond*ts+notFullParticles
notFullParticles=requiredParticleCnt % 1
requiredParticleCnt=math.floor(requiredParticleCnt)

particleVelocity=(6*ts*totalExertedForce)/(requiredParticleCnt*particleDensity*math.pi*particleSize*particleSize*particleSize)

maxParticleDeviation=math.tan(particleScatteringAngle*0.5*math.pi/180)*particleVelocity

while (particleCnt<requiredParticleCnt) do
	-- we want a uniform distribution:
	x=(math.random()-0.5)*2
	y=(math.random()-0.5)*2
	if (x*x+y*y<=1) then
		if (simulateParticles) then
			pos[1]=x*0.08*s
			pos[2]=y*0.08*s
			pos[3]=-particleSize*0.6
			dir[1]=pos[1]+(math.random()-0.5)*maxParticleDeviation*2
			dir[2]=pos[2]+(math.random()-0.5)*maxParticleDeviation*2
			dir[3]=pos[3]-particleVelocity*(1+0.2*(math.random()-0.5))
			pos=simMultiplyVector(m,pos)
			dir=simMultiplyVector(m,dir)
			itemData={pos[1],pos[2],pos[3],dir[1],dir[2],dir[3]}
			simAddParticleObjectItem(particleObject,itemData)
		end
		particleCnt=particleCnt+1
	end
end
-- Apply a reactive force onto the body:
--totalExertedForce=particleCnt*particleDensity*particleVelocity*math.pi*particleSize*particleSize*particleSize/(6*ts)
force={0,0,totalExertedForce}
rm=simBuildMatrix({0,0,0},simGetObjectOrientation(heli, -1))
force=simMultiplyVector(rm, force)

m[4]=0
m[8]=0
m[12]=0
torque={0,0,0.002*particleVelocity}
torque=simMultiplyVector(m,torque)
simAddForceAndTorque(propellerRespondable,force,torque)

-- Now make sure that child scripts attached on this hierarchy are also executed:
simHandleChildScript(sim_handle_all_except_explicit)
