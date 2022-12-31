use bevy::prelude::*;
use bevy::render::mesh::{Indices, PrimitiveTopology, VertexAttributeValues};

#[derive(Component)]
pub struct LightMarker;

pub struct Light {
    pub epoch: Vec2,
    pub head: Vec2,
    pub direction: Vec2,
}

impl Light {
    pub fn new(epoch: Vec2) -> Self {
        Light {
            epoch,
            head: epoch,
            direction: Vec2::X,
        }
    }

    pub fn set_direction(&mut self, direction: Vec2) {
        self.direction = direction;
    }

    pub fn move_head(&mut self, factor: f32) {
        self.head += self.direction.normalize() * factor;
    }

    pub fn reset(&mut self) {
        self.head = self.epoch;
        self.direction = Vec2::X;
    }

    pub fn add_point_to_mesh(&self, mesh: &mut Mesh) {
        let count = mesh.count_vertices();
        if let Some(VertexAttributeValues::Float32x3(positions)) =
            mesh.attribute_mut(Mesh::ATTRIBUTE_POSITION)
        {
            positions.push([self.head.x, self.head.y, 0.]);
        }
        if let Some(VertexAttributeValues::Float32x3(normals)) =
            mesh.attribute_mut(Mesh::ATTRIBUTE_NORMAL)
        {
            normals.push([0., 0., 1.]);
        }
        if let Some(VertexAttributeValues::Float32x2(uvs)) =
            mesh.attribute_mut(Mesh::ATTRIBUTE_UV_0)
        {
            uvs.push([0., 0.]);
        }
        if let Some(Indices::U32(indices)) = mesh.indices_mut() {
            indices.push(count as u32);
        }
    }
}

impl From<&Light> for Mesh {
    fn from(light: &Light) -> Mesh {
        let vertices = [([light.epoch.x, light.epoch.y, 0.], [0., 0., 1.], [0., 0.])];
        let indices = Indices::U32(vec![0]);
        let positions: Vec<_> = vertices.iter().map(|(p, _, _)| *p).collect();
        let normals: Vec<_> = vertices.iter().map(|(_, p, _)| *p).collect();
        let uvs: Vec<_> = vertices.iter().map(|(_, _, uv)| *uv).collect();
        let mut mesh = Mesh::new(PrimitiveTopology::LineStrip);
        mesh.set_indices(Some(indices));
        mesh.insert_attribute(Mesh::ATTRIBUTE_POSITION, positions);
        mesh.insert_attribute(Mesh::ATTRIBUTE_NORMAL, normals);
        mesh.insert_attribute(Mesh::ATTRIBUTE_UV_0, uvs);
        mesh
    }
}
