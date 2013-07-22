if (simGetScriptExecutionCount()==0) then
	-- Make sure we have version 2.4.13 or above (the particles are not supported otherwise)
	v=simGetIntegerParameter(sim_intparam_program_version)
	if (v<20413) then
		simDisplayDialog('Warning','The propeller model is only fully supported from V-REP version 2.4.13 and above.&&nThis simulation will not run as expected!',sim_dlgstyle_ok,false,'',nil,{0.8,0,0,0,0,0})
	end

	-- Detatch the manipulation sphere:
	targetObj=simGetObjectHandle('Quadricopter_target')
	simSetObjectParent(targetObj,-1,true)

	-- This control algo was quickly written and is dirty and not optimal. It just serves as a SIMPLE example

	d=simGetObjectHandle('Quadricopter_base')

	particlesAreVisible=simGetScriptSimulationParameter(sim_handle_self,'particlesAreVisible')
	simSetScriptSimulationParameter(sim_handle_tree,'particlesAreVisible',tostring(particlesAreVisible))
	
	simulateParticles=simGetScriptSimulationParameter(sim_handle_self,'simulateParticles')
	simSetScriptSimulationParameter(sim_handle_tree,'simulateParticles',tostring(simulateParticles))

	propellerScripts={-1,-1,-1,-1}
	for i=1,4,1 do
		propellerScripts[i]=simGetScriptHandle('Quadricopter_propeller_respondable'..i)
	end
	heli=simGetObjectAssociatedWithScript(sim_handle_self)

	propForces={0,0,0,0}
	force={0,0,0,0}
	propCentres={{0,0,0}
				,{0,0,0}
				,{0,0,0}
				,{0,0,0}}

	fakeShadow=simGetScriptSimulationParameter(sim_handle_self,'fakeShadow')
	if (fakeShadow) then
		shadowCont=simAddDrawingObject(sim_drawing_discpoints+sim_drawing_cyclic+sim_drawing_25percenttransparency+sim_drawing_50percenttransparency+sim_drawing_itemsizes,0.2,0,-1,1)
	end

	-- Prepare 2 floating views with the camera views:
	floorCam=simGetObjectHandle('Quadricopter_floorCamera')
	frontCam=simGetObjectHandle('Quadricopter_frontCamera')
	floorView=simFloatingViewAdd(0.9,0.9,0.2,0.2,0)
	frontView=simFloatingViewAdd(0.7,0.9,0.2,0.2,0)
	simAdjustView(floorView,floorCam,64)
	simAdjustView(frontView,frontCam,64)
end

s=simGetObjectSizeFactor(d)

pos=simGetObjectPosition(d,-1)
if (fakeShadow) then
	itemData={pos[1],pos[2],0.002,0,0,1,0.2*s}
	simAddDrawingObjectItem(shadowCont,itemData)
end

mass,_,_ = simGetShapeMassAndInertia(heli)

gravity = simGetArrayParameter(sim_arrayparam_gravity)

print("Mass:")
print(mass)

for i=1,4,1 do
	prop=simGetObjectAssociatedWithScript(propellerScripts[i])
	tmpMass,_,propCentres[i]=simGetShapeMassAndInertia(prop)
	mass=mass+tmpMass
	print(mass)
end

length=propCentres[1][1]-propCentres[2][1]
2

--QC is facing along positive X

force[3] = -(gravity[3]) * mass

print("Force:")
print(force[3])

propForces[1]=force[3]/4
propForces[2]=force[3]/4 + force[2] / 2
propForces[3]=force[3]/4 + force[2] / 2
propForces[4]=force[3]/4

-- Send the desired motor velocities to the 4 rotors:
for i=1,4,1 do
	simSetScriptSimulationParameter(propellerScripts[i],'totalForce',propForces[i])
end
simHandleChildScript(sim_handle_all_except_explicit)


if (simGetSimulationState()==sim_simulation_advancing_lastbeforestop) then
	-- Now reset the manipulation sphere:
	simSetObjectParent(targetObj,d,true)
	simSetObjectPosition(targetObj,sim_handle_parent,{0,0,0})
	simSetObjectOrientation(targetObj,sim_handle_parent,{0,0,0})
end
