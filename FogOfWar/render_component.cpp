#include "render_component.h"



void RenderComponent::render(TransformComponent* transform, Pipeline& p, Renderer* r)
{
	p.pushMatrix();
		p.translate(transform->getPosition());
		p.addMatrix(transform->getRotation());
		p.scale(transform->getScale());
		r->setUniLocs(p);
		m_model->render();
	p.popMatrix();
}


bool RenderComponent::canRender()
{
	return m_model != NULL;
}


void RenderComponent::setModel(Model* model)
{
	m_model = model;
}

void RenderComponent::clearModel()
{
	// we don't delete it here, cuz model manager owns the actual models
	m_model = NULL;
}
