from app.utility.base_world import BaseWorld
from plugins.dnp3.app.dnp3_svc import Dnp3Service

name = 'DNP3'
description = 'The DNP3 plugin for Caldera provides adversary emulation abilities specific to the DNP3 control systems protocol.'
address = '/plugin/dnp3/gui'
access = BaseWorld.Access.RED


async def enable(services):
    dnp3_svc = Dnp3Service(services, name, description)
    app = services.get('app_svc').application
    app.router.add_route('GET', '/plugin/dnp3/gui', dnp3_svc.splash)
    app.router.add_route('GET', '/plugin/dnp3/data', dnp3_svc.plugin_data)