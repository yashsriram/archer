use bevy::prelude::*;

#[derive(Component)]
pub struct TargetMarker;

pub struct Target {
    pub center: Vec2,
    pub radius: f32,
}
