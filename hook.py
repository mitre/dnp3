from app.utility.base_world import BaseWorld
from plugins.dnp3.app.dnp3_gui import Dnp3GUI
from plugins.dnp3.app.dnp3_api import Dnp3API

name = 'DNP3'
description = 'The DNP3 plugin for Caldera provides adversary emulation abilities specific to the DNP3 control systems protocol.'
address = '/plugin/dnp3/gui'
access = BaseWorld.Access.RED


async def enable(services):
    app = services.get('app_svc').application
    dnp3_gui = Dnp3GUI(services, name=name, description=description)
    app.router.add_static('/dnp3', 'plugins/dnp3/static/', append_version=True)
    app.router.add_route('GET', '/plugin/dnp3/gui', dnp3_gui.splash)

    dnp3_api = Dnp3API(services)
    # Add API routes here
    app.router.add_route('POST', '/plugin/dnp3/mirror', dnp3_api.mirror)

