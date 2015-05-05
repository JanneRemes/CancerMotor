#include "PhysicsSystem.h"
#include "GameObject.h"
#include "Transformable.h"
#include <math.h>

PhysicsSystem::PhysicsSystem(float grav)
{
	gravity = grav;
	deltaTime = 0;
}

void PhysicsSystem::Update(double deltaT)
{
	deltaTime = deltaT;

	PCIT = physicsComponents.begin();
	while (PCIT != physicsComponents.end())
	{
		UpdateForces((*PCIT));
		Collision((*PCIT));
		UpdateSpeed((*PCIT));
		PCIT++;
	}
}

void PhysicsSystem::UpdateSpeed(Physics* comp)
{
	comp->SetSpeed(Vector3<float>(
		comp->GetSpeed().x + (comp->GetForces().x * deltaTime / comp->GetMass() ),	//X
		comp->GetSpeed().y + (comp->GetForces().y * deltaTime / comp->GetMass() ),	//Y
		comp->GetSpeed().z + (comp->GetForces().z * deltaTime / comp->GetMass() )	//Z
		));

	comp->SetForces(Vector3<float>(
		0.0f,
		0.0f,
		0.0f
		));

	GameObject* GO = (GameObject*)comp->GetParent();

	if (GO->GetComponent<Transformable>()->GetOrigin()[1] > 80)
		comp->SetSpeed(Vector3<float>(comp->GetSpeed().x, -std::abs(comp->GetSpeed().y * comp->GetElasticity()), comp->GetSpeed().z));
	if (GO->GetComponent<Transformable>()->GetOrigin()[1] < -80)
		comp->SetSpeed(Vector3<float>(comp->GetSpeed().x, std::abs(comp->GetSpeed().y * comp->GetElasticity()), comp->GetSpeed().z));
	if (GO->GetComponent<Transformable>()->GetOrigin()[0] < -120)
		comp->SetSpeed(Vector3<float>(std::abs(comp->GetSpeed().x * comp->GetElasticity()), comp->GetSpeed().y, comp->GetSpeed().z));
	if (GO->GetComponent<Transformable>()->GetOrigin()[0] >  120)
		comp->SetSpeed(Vector3<float>(-std::abs(comp->GetSpeed().x * comp->GetElasticity()), comp->GetSpeed().y, comp->GetSpeed().z));

	GO->GetComponent<Transformable>()->SetOrigin(
		GO->GetComponent<Transformable>()->GetOrigin()[0] + comp->GetSpeed().x * deltaTime,
		GO->GetComponent<Transformable>()->GetOrigin()[1] + comp->GetSpeed().y * deltaTime,
		GO->GetComponent<Transformable>()->GetOrigin()[2] + comp->GetSpeed().z * deltaTime
		);

}

void PhysicsSystem::UpdateForces(Physics* comp)
{
	comp->SetForces(Vector3<float>(
		comp->GetForces().x,
		comp->GetForces().y - gravity * comp->GetMass(),
		comp->GetForces().z
		));


}

void PhysicsSystem::Collision(Physics* comp)
{
	GameObject* GO = (GameObject*)comp->GetParent();
	int fo = 0;
	int to = 0;
	PCIT2 = physicsComponents.begin();

	while (PCIT2 != physicsComponents.end())
	{
		if ((*PCIT2)->componentId == comp->componentId)
			PCIT2++;
		//else if (fo == comp->componentId && to == (*PCIT2)->componentId)
		//{
		//	PCIT2++;
		//	fo = 0;
		//	to = 0;
		//}
		else
		{
			GameObject* GO2 = (GameObject*)(*PCIT2)->GetParent();

			float al = GO->GetComponent<Transformable>()->GetOrigin()[0] - GO2->GetComponent<Transformable>()->GetOrigin()[0] + comp->GetSpeed().x * deltaTime + (*PCIT2)->GetSpeed().x * deltaTime;
			float ad = GO->GetComponent<Transformable>()->GetOrigin()[1] - GO2->GetComponent<Transformable>()->GetOrigin()[1] + comp->GetSpeed().y * deltaTime + (*PCIT2)->GetSpeed().y * deltaTime;
			if (sqrt(al * al + ad * ad) < 60)
			{
				std::cout << "collision" << std::endl;
				Vector3<float> asd(GO->GetComponent<Transformable>()->GetOrigin()[0], GO->GetComponent<Transformable>()->GetOrigin()[1], GO->GetComponent<Transformable>()->GetOrigin()[2]);

				Vector3<float> asd2(GO2->GetComponent<Transformable>()->GetOrigin()[0], GO2->GetComponent<Transformable>()->GetOrigin()[1], GO2->GetComponent<Transformable>()->GetOrigin()[2]);

				float angle = atan((asd.x - asd2.x) / (asd.y - asd2.y));

				float AbN = comp->GetSpeed().x * cos(angle) + comp->GetSpeed().y * sin(angle);
				float AbT = comp->GetSpeed().x * -sin(angle) + comp->GetSpeed().y * cos(angle);
				float BbN = (*PCIT2)->GetSpeed().x * cos(angle) + (*PCIT2)->GetSpeed().y * sin(angle);
				float BbT = (*PCIT2)->GetSpeed().x * -sin(angle) + (*PCIT2)->GetSpeed().y * cos(angle);

				float AaN = ((comp->GetMass() - comp->GetElasticity() * (*PCIT2)->GetMass()) / (comp->GetMass() + (*PCIT2)->GetMass())) * AbN;
				AaN += (((1 + comp->GetElasticity()) * (*PCIT2)->GetMass()) / (comp->GetMass() + (*PCIT2)->GetMass())) * BbN;
				float BaN = ((comp->GetMass() - (*PCIT2)->GetElasticity() * (*PCIT2)->GetMass()) / (comp->GetMass() + (*PCIT2)->GetMass())) * BbN;
				BaN += (((1 + comp->GetElasticity()) * (*PCIT2)->GetMass()) / (comp->GetMass() + (*PCIT2)->GetMass())) * AbN;

				comp->SetSpeed(Vector3<float>(AaN * cos(angle) + AbT * -sin(angle), AaN * sin(angle) + AbT * cos(angle), comp->GetSpeed().z));
				(*PCIT2)->SetSpeed(Vector3<float>(BaN * cos(angle) + BbT * -sin(angle), BaN * sin(angle) + BbT * cos(angle), comp->GetSpeed().z));
				fo = comp->componentId;
				to = (*PCIT2)->componentId;
			}
			PCIT2++;
		}
			
	}

}

void PhysicsSystem::AddPhysicsComponent(Physics* physComp)
{
	physicsComponents.push_back(physComp);
}